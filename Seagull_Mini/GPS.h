#pragma once
#include "Arduino.h"
#include <NMEAGPS.h>
#include "AutopilotControls.h"
#include "Board.h"




NMEAGPS gps;
gps_fix fix;
FireTimer lossGPSTimer;




#define LOSS_GPS_TIMEOUT 1000
#define GPS_DEFAULT_PORT_BAUD 38400
#define GPS_PORT_BAUD 115200
#define GPS_REFRESH   10
#define GPGGA_ENABLED 1
#define GPGLL_ENABLED 0
#define GPGSV_ENABLED 0
#define GPGSA_ENABLED 0
#define GPRMC_ENABLED 1
#define GPVTG_ENABLED 0
#define GNGRS_ENABLED 0
#define GNGST_ENABLED 0
#define GNZDA_ENABLED 0
#define GNGBS_ENABLED 0
#define GNDTM_ENABLED 0

#define NMEA_LEN 16
#define FREQ_LEN 14
#define BAUD_LEN 28

#define GPGGA 0
#define GPGLL 1
#define GPGSA 2
#define GPGSV 3
#define GPRMC 4
#define GPVTG 5
#define GNGRS 6
#define GNGST 7
#define GNZDA 8
#define GNGBS 9
#define GNDTM 10

#define NMEA_ID_POS  7
#define DDC_POS      8
#define SERIAL_1_POS 9
#define SERIAL_2_POS 10
#define USB_POS      11
#define SPI_POS      12

#define BAUD_0 14
#define BAUD_1 15
#define BAUD_2 16
#define BAUD_3 17

#define MEAS_RATE_1 6
#define MEAS_RATE_2 7
#define NAV_RATE_1  8
#define NAV_RATE_2  9

#define LIMITER_PERIOD 20




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




bool gpsConnected;




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




void readGPSData()
{
  if (fix.valid.status)
  {
    if (fix.valid.location)
    {
      plane.prev_lat = plane.lat;
      plane.prev_lon = plane.lon;
      
      plane.lat = fix.latitude();
      plane.lon = fix.longitude();

      plane.cog_gps_calc = find_heading(plane.prev_lat, plane.prev_lon, plane.lat, plane.lon);
      plane.ias_gps_calc = find_distance(plane.prev_lat, plane.prev_lon, plane.lat, plane.lon) / (3600000.0 / lossGPSTimer.timeDiff); // convert ms to hr

      //record that fix data is valid
      validFlags |= FIX_VALID;
    }
    else
    {
      plane.lat = 0;
      plane.lon = 0;

      plane.cog_gps_calc = 0;
      plane.ias_gps_calc = 0;

      // record that fix data is not valid
      validFlags &= ~(byte)FIX_VALID;
    }

    if (fix.valid.date)
    {
      plane.UTC_year  = fix.dateTime.year;
      plane.UTC_month = fix.dateTime.month;
      plane.UTC_day   = fix.dateTime.date;
    }
    else
    {
      plane.UTC_year  = 0;
      plane.UTC_month = 0;
      plane.UTC_day   = 0;
    }
    
    if (fix.valid.time)
    {
      plane.UTC_hour   = fix.dateTime.hours;
      plane.UTC_minute = fix.dateTime.minutes;
      plane.UTC_second = fix.dateTime.seconds;
    }
    else
    {
      plane.UTC_hour   = 0;
      plane.UTC_minute = 0;
      plane.UTC_second = 0;
    }

    if (fix.valid.speed)
      plane.ias_gps = fix.speed_kph();
    else
      plane.ias_gps = 0;

    if (fix.valid.heading)
      plane.cog_gps = fix.heading();
    else
      plane.cog_gps = 0;

    if(fix.valid.altitude)
      plane.alt_gps = fix.altitude();
    else
      plane.alt_gps = 0;
  }
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




void setupGPS()
{
  GPS_PORT.begin(GPS_DEFAULT_PORT_BAUD);
  
  lossGPSTimer.begin(LOSS_GPS_TIMEOUT);
  
  //disable all NMEA sentences first
  setSentence(GPGGA, false);
  setSentence(GPGLL, false);
  setSentence(GPGSA, false);
  setSentence(GPGSV, false);
  setSentence(GPRMC, false);
  setSentence(GPVTG, false);
  setSentence(GNGRS, false);
  setSentence(GNGST, false);
  setSentence(GNZDA, false);
  setSentence(GNGBS, false);
  setSentence(GNDTM, false);

  //then select the ones you want
  if (GPGGA_ENABLED)
    setSentence(GPGGA, true);
  if (GPGLL_ENABLED)
    setSentence(GPGLL, true);
  if (GPGSV_ENABLED)
    setSentence(GPGSV, true);
  if (GPGSA_ENABLED)
    setSentence(GPGSA, true);
  if (GPRMC_ENABLED)
    setSentence(GPRMC, true);
  if (GPVTG_ENABLED)
    setSentence(GPVTG, true);
  if (GNGRS_ENABLED)
    setSentence(GNGRS, true);
  if (GNGST_ENABLED)
    setSentence(GNGST, true);
  if (GNZDA_ENABLED)
    setSentence(GNZDA, true);
  if (GNGBS_ENABLED)
    setSentence(GNGBS, true);
  if (GNDTM_ENABLED)
    setSentence(GNDTM, true);

  changeFreq(GPS_REFRESH);

  if (GPS_PORT_BAUD != GPS_DEFAULT_PORT_BAUD)
    changeBaud(GPS_PORT_BAUD);
}




bool gpsFailover()
{
  if (lossGPSTimer.fire(false))
  {
    gpsConnected = false;
    validFlags &= ~(byte)FIX_RECENT;
  }

  return gpsConnected;
}




bool pollGPS()
{
  if (gps.available(GPS_PORT))
  {
    fix = gps.read();
    readGPSData();

    lossGPSTimer.start();
    gpsConnected = true;
    validFlags |= FIX_RECENT;
  }

  return gpsFailover();
}
