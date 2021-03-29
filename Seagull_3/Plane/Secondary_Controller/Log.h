#pragma once
#include "SdTerminal.h"
#include "PacketStructs.h"




// Uncomment 'USE_EXTERNAL_SD' define to use an external SD card adapter or leave
// it commented to use the built in sd card.
//#define USE_EXTERNAL_SD 

#ifdef USE_EXTERNAL_SD
const uint8_t SD_CS_PIN = SS;
#define SPI_CLOCK SD_SCK_MHZ(10)
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)
#else // Use built in SD card

#ifdef SDCARD_SS_PIN
const uint8_t SD_CS_PIN = SDCARD_SS_PIN;
#endif // SDCARD_SS_PIN
#define SPI_CLOCK SD_SCK_MHZ(50)
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#endif // USE_EXTERNAL_SD




const char nameTemplate[] = "flight_%d.txt";
const char headerRow[]    = "epoch_ms,alt_cm,course_angle_imu,roll_deg,pitch_deg,pitot_pressure,valid_flags,lat_dd,lon_dd,year,month,day,hour,min,sec,sog,cog,throttle,pitch,yaw,roll";




SdFs sd;
FsFile myFile;
Terminal myTerminal;




char target[] = "%d,%d,%s,%s,%s,%d,%d,%s,%s,%d,%d,%d,%d,%d,%s,%s,%s,%d,%d,%d,%d";
char filename[20];




void setupLog()
{
  unsigned int flightCount = 1;

  while (!sd.begin(SD_CONFIG))
  {
    Serial.println(F("SD iniatialization failed"));
    delay(100);
  }

  sprintf(filename, "flight_%d.txt", flightCount);

  while(sd.exists(filename))
  {
    flightCount++;
    sprintf(filename, "flight_%d.txt", flightCount);
  }
  
  myFile.open(filename, FILE_WRITE);
  myFile.println(headerRow);
  myFile.close();

  myTerminal.begin(sd);
}




void logData()
{
  char str_coAngIMU[15];
  char str_roll[15];
  char str_pitch[15];
  char str_lat[15];
  char str_lon[15];
  char str_sec[15];
  char str_sog[15];
  char str_cog[15];

  char buff[150];

  dtostrf(telem.courseAngleIMU,   4, 2, str_coAngIMU);
  dtostrf(telem.rollAngle,        4, 6, str_roll);
  dtostrf(telem.pitchAngle,       4, 6, str_pitch);
  dtostrf(telem.latitude,         4, 7, str_lat);
  dtostrf(telem.longitude,        4, 7, str_lon);
  dtostrf(telem.UTC_second,       4, 2, str_sec);
  dtostrf(telem.speedOverGround,  4, 2, str_sog);
  dtostrf(telem.courseOverGround, 4, 2, str_cog);

  sprintf(buff,
          target,
          millis(),
          telem.altitude,
          str_coAngIMU,
          str_roll,
          str_pitch,
          telem.pitotPressure,
          telem.validFlags,
          str_lat,
          str_lon,
          telem.UTC_year,
          telem.UTC_month,
          telem.UTC_day,
          telem.UTC_hour,
          telem.UTC_minute,
          str_sec,
          str_sog,
          str_cog,
          inputs.throttle,
          inputs.pitch,
          inputs.yaw,
          inputs.roll);

  if (!sd.exists(filename))
    setupLog();

  myFile.open(filename, FILE_WRITE);
  myFile.println(buff);
  myFile.close();
}




void handleTerminal()
{
  myTerminal.handleCmds();
}
