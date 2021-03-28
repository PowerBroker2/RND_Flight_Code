#pragma once
#include "NMEAGPS.h"
#include "FireTimer.h"
#include "PacketStructs.h"




#define GPS_PORT Serial4




const int NMEA_LEN = 16;
const int FREQ_LEN = 14;
const int BAUD_LEN = 28;

const int GPGGA = 0;
const int GPGLL = 1;
const int GPGLV = 2;
const int GPGSA = 3;
const int GPRMC = 4;
const int GPVTG = 5;

const int NMEA_ID_POS  = 7;
const int DDC_POS      = 8;
const int SERIAL_1_POS = 9;
const int SERIAL_2_POS = 10;
const int USB_POS      = 11;
const int SPI_POS      = 12;

const int BAUD_0 = 14;
const int BAUD_1 = 15;
const int BAUD_2 = 16;
const int BAUD_3 = 17;

const int MEAS_RATE_1 = 6;
const int MEAS_RATE_2 = 7;
const int NAV_RATE_1  = 8;
const int NAV_RATE_2  = 9;

const int GPS_TIMEOUT   = 1000; // ms
const int GPS_BAUD      = 115200;
const int GPS_REFRESH   = 10;   // Hz
const int GPGGA_ENABLED = 0;
const int GPGLL_ENABLED = 0;
const int GPGLV_ENABLED = 0;
const int GPGSA_ENABLED = 0;
const int GPRMC_ENABLED = 1;
const int GPVTG_ENABLED = 0;




const char CFG_MSG[NMEA_LEN] = {
  0xB5, // Header char 1
  0x62, // Header char 2
  0x06, // class
  0x01, // id
  0x08, // length LSB
  0x00, // length MSB
  0xF0, // payload (NMEA sentence ID char 1)
  0x00, // payload (NMEA sentence ID char 2)
  0x00, // payload I/O Target 0 - DDC           - (1 - enable sentence, 0 - disable)
  0x00, // payload I/O Target 1 - Serial Port 1 - (1 - enable sentence, 0 - disable)
  0x00, // payload I/O Target 2 - Serial Port 2 - (1 - enable sentence, 0 - disable)
  0x00, // payload I/O Target 3 - USB           - (1 - enable sentence, 0 - disable)
  0x00, // payload I/O Target 4 - SPI           - (1 - enable sentence, 0 - disable)
  0x00, // payload I/O Target 5 - Reserved      - (1 - enable sentence, 0 - disable)
  0x00, // CK_A
  0x00  // CK_B
};

const char CFG_RATE[FREQ_LEN] = {
  0xB5, // sync char 1
  0x62, // sync char 2
  0x06, // class
  0x08, // id
  0x06, // length LSB
  0x00, // length MSB
  0x64, // payload measRate (ms) 1
  0x00, // payload measRate (ms) 2
  0x00, // payload navRate (cycles) 1
  0x00, // payload navRate (cycles) 2
  0x01, // payload timeRef 1
  0x00, // payload timeRef 2
  0x00, // CK_A
  0x00  // CK_B
};

const char CFG_PRT[BAUD_LEN] = {
  0xB5, // sync char 1
  0x62, // sync char 2
  0x06, // class
  0x00, // id
  0x14, // length LSB
  0x00, // length MSB
  0x01, // payload portID
  0x00, // payload reserved0
  0x00, // payload txReady 1
  0x00, // payload txReady 2
  0xD0, // payload mode 1
  0x08, // payload mode 2
  0x00, // payload mode 3
  0x00, // payload mode 4
  0x00, // payload baudRate 0 (LSB)
  0x00, // payload baudRate 1
  0x00, // payload baudRate 2
  0x00, // payload baudRate 3 (MSB)
  0x07, // payload inProtoMask 1
  0x00, // payload inProtoMask 2
  0x03, // payload outProtoMask 1
  0x00, // payload outProtoMask 2
  0x00, // payload reserved4 1
  0x00, // payload reserved4 2
  0x00, // payload reserved5 1
  0x00, // payload reserved5 2
  0x00, // CK_A
  0x00  // CK_B
};




uint32_t bauds[12] = { 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 115200 };
NMEAGPS gps;
gps_fix fix;
FireTimer lossGPSTimer;
FireTimer autobaudTimer;




void insertChecksum(char packet[], const byte len)
{
  uint8_t ck_a = 0;
  uint8_t ck_b = 0;

  // exclude the first and last two bytes in packet
  for (byte i = 2; i < (len - 2); i++)
  {
    ck_a += packet[i];
    ck_b += ck_a;
  }

  packet[len - 2] = ck_a;
  packet[len - 1] = ck_b;
}




void changeBaud(uint32_t baud)
{
  char configPacket[BAUD_LEN];
  memcpy(configPacket, CFG_PRT, BAUD_LEN);

  configPacket[BAUD_0] = (char)(baud & 0xFF);
  configPacket[BAUD_1] = (char)((baud >> 8) & 0xFF);
  configPacket[BAUD_2] = (char)((baud >> 16) & 0xFF);
  configPacket[BAUD_3] = (char)((baud >> 24) & 0xFF);

  insertChecksum(configPacket, BAUD_LEN);
  GPS_PORT.write(configPacket, BAUD_LEN);

  delay(100);

  GPS_PORT.flush();
  GPS_PORT.begin(baud);
}




uint32_t findBaud()
{
  for (uint8_t i = 0; i < (sizeof(bauds) / sizeof(bauds[0])); i++)
  {
    bool found = false;
    GPS_PORT.begin(bauds[i]);

    delay(100);

    while (GPS_PORT.available())
      GPS_PORT.read();

    autobaudTimer.begin(100);

    while (!autobaudTimer.fire())
    {
      if (GPS_PORT.available() >= 2)
      {
        char temp  = GPS_PORT.read();
        char temp2 = GPS_PORT.read();

        if (temp == '$' && temp2 == 'G')
        {
          found = true;
          break;
        }
      }
    }

    if (found)
      return bauds[i];
  }

  return 38400;
}




void setSentence(char NMEA_num, bool enable)
{
  char configPacket[NMEA_LEN];
  memcpy(configPacket, CFG_MSG, NMEA_LEN);

  if (enable)
    configPacket[SERIAL_1_POS] = 1;

  configPacket[NMEA_ID_POS] = NMEA_num;
  insertChecksum(configPacket, NMEA_LEN);

  GPS_PORT.write(configPacket, NMEA_LEN);
}




void changeFreq(uint16_t hertz)
{
  uint16_t normHerz = hertz / (1000 / ((CFG_RATE[MEAS_RATE_2] << 8) | CFG_RATE[MEAS_RATE_1]));
  char configPacket[FREQ_LEN];
  memcpy(configPacket, CFG_RATE, FREQ_LEN);

  configPacket[NAV_RATE_1] = (char)(normHerz & 0xFF);
  configPacket[NAV_RATE_2] = (char)((normHerz >> 8) & 0xFF);

  insertChecksum(configPacket, FREQ_LEN);
  GPS_PORT.write(configPacket, FREQ_LEN);
}




void setupGPS()
{
  lossGPSTimer.begin(GPS_TIMEOUT);

  int initBaud = findBaud();
  
  //disable all NMEA sentences first
  setSentence(GPGGA, false);
  setSentence(GPGLL, false);
  setSentence(GPGLV, false);
  setSentence(GPGSA, false);
  setSentence(GPRMC, false);
  setSentence(GPVTG, false);

  //then select the ones you want
  if (GPGGA_ENABLED)
    setSentence(GPGGA, true);
  if (GPGLL_ENABLED)
    setSentence(GPGLL, true);
  if (GPGLV_ENABLED)
    setSentence(GPGLV, true);
  if (GPGSA_ENABLED)
    setSentence(GPGSA, true);
  if (GPRMC_ENABLED)
    setSentence(GPRMC, true);
  if (GPVTG_ENABLED)
    setSentence(GPVTG, true);

  changeFreq(GPS_REFRESH);

  if (initBaud != GPS_BAUD)
    changeBaud(GPS_BAUD);
}




void gpsFailover()
{
  if (lossGPSTimer.fire(false))
    telem.validFlags = telem.validFlags & ~(byte)0x8;
}




void readGPSData()
{
  if (fix.valid.status)
  {
    if (fix.valid.location)
    {
      telem.latitude  = fix.latitude();
      telem.longitude = fix.longitude();

      //record that fix data is valid
      telem.validFlags = telem.validFlags | 0x6;
    }
    else
    {
      telem.latitude  = 0;
      telem.longitude = 0;

      //record that fix data is not valid
      telem.validFlags = telem.validFlags & ~(byte)0x6;
    }

    if (fix.valid.date)
    {
      telem.UTC_year  = fix.dateTime.year;
      telem.UTC_month = fix.dateTime.month;
      telem.UTC_day   = fix.dateTime.date;
    }
    else
    {
      telem.UTC_year  = 0;
      telem.UTC_month = 0;
      telem.UTC_day   = 0;
    }
    
    if (fix.valid.time)
    {
      telem.UTC_hour   = fix.dateTime.hours;
      telem.UTC_minute = fix.dateTime.minutes;
      telem.UTC_second = fix.dateTime.seconds;
    }
    else
    {
      telem.UTC_hour   = 0;
      telem.UTC_minute = 0;
      telem.UTC_second = 0;
    }

    if (fix.valid.speed)
      telem.speedOverGround = fix.speed_kph();
    else
      telem.speedOverGround = 0;

    if (fix.valid.heading)
      telem.courseOverGround = fix.heading();
    else
      telem.courseOverGround = 0;
  }
}




void pollGPS()
{
  if (gps.available(GPS_PORT))
  {
    fix = gps.read();
    readGPSData();

    lossGPSTimer.start();
    telem.validFlags = telem.validFlags | 0x8;
  }

  gpsFailover();
}
