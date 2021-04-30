#pragma once
#include "Arduino.h"
#include "SdFat.h"
#include "SdTerminal.h"
#include "IMU.h"
#include "Controls.h"
#include "Board.h"




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




const char nameTemplate[] = "flight_%d.csv";
const char headerRow[] = "Loop_Frequency,"
                         "Engage_Autopilot,"
                         "Valid_Flags,"
                         "Altitude_GPS,"
                         "Lat,"
                         "Lon,"
                         "SOG_GPS,"
                         "SOG_GPS_CALC,"
                         "COG_GPS,"
                         "COG_GPS_CALC,"
                         "Heading_Compass,"
                         "Heading_IMU,"
                         "Pitch,"
                         "Roll,"
                         "Elevator_Pulse_Len,"
                         "Aileron_Pulse_Len,"
                         "Rudder_Pulse_Len,"
                         "Throttle_Pulse_Len";




SdFs sd;
FsFile myFile;
Terminal myTerminal;




bool useSD = false;
char filename[20];
int dataIndex = 0;




void setupLog()
{
  if(useSD)
  {
    unsigned int flightCount = 1;
    
    if(!sd.begin(SD_CONFIG))
    {
      useSD = false;
      Serial.println(F("SD iniatialization failed"));
      delay(100);
    }
  
    sprintf(filename, "flight_%d.csv", flightCount);
  
    while(sd.exists(filename))
    {
      flightCount++;
      sprintf(filename, "flight_%d.csv", flightCount);
    }
    
    myFile = sd.open(filename, FILE_WRITE);
    myFile.println(headerRow);
    myFile.close();
    myFile = sd.open(filename, O_WRITE | O_APPEND);
  }
}




void setupSD()
{
  if(useSD)
  {
    setupLog();
    myTerminal.begin(sd);
  }
}




void addToBuff(char buff[], int& buffIndex, char data[], bool newLine = false)
{
  memcpy(buff + buffIndex, data, strlen(data));
  buffIndex += strlen(data);
  
  if(newLine)
    buff[buffIndex] = '\n';
  else
    buff[buffIndex] = ',';
    
  buffIndex++;
}




void logData()
{
  char buff[300];
  int buffIndex = 0;

  char str_loopFreq[5];
  dtostrf(loopFreq, 4, 0, str_loopFreq);
  addToBuff(buff, buffIndex, str_loopFreq);

  char str_engageAP[5];
  dtostrf(engageAP, 4, 0, str_engageAP);
  addToBuff(buff, buffIndex, str_engageAP);

  char str_validFlags[5];
  dtostrf(validFlags, 4, 0, str_validFlags);
  addToBuff(buff, buffIndex, str_validFlags);
  
  char str_alt_gps[15];
  dtostrf(plane.alt_gps, 4, 10, str_alt_gps);
  addToBuff(buff, buffIndex, str_alt_gps);

  char str_lat[15];
  dtostrf(plane.lat, 4, 10, str_lat);
  addToBuff(buff, buffIndex, str_lat);

  char str_lon[15];
  dtostrf(plane.lon, 4, 10, str_lon);
  addToBuff(buff, buffIndex, str_lon);

  char str_ias_gps[15];
  dtostrf(plane.ias_gps, 4, 10, str_ias_gps);
  addToBuff(buff, buffIndex, str_ias_gps);

  char str_ias_gps_calc[15];
  dtostrf(plane.ias_gps_calc, 4, 10, str_ias_gps_calc);
  addToBuff(buff, buffIndex, str_ias_gps_calc);

  char str_cog_gps[15];
  dtostrf(plane.cog_gps, 4, 10, str_cog_gps);
  addToBuff(buff, buffIndex, str_cog_gps);

  char str_cog_gps_calc[15];
  dtostrf(plane.cog_gps_calc, 4, 10, str_cog_gps_calc);
  addToBuff(buff, buffIndex, str_cog_gps_calc);

  char str_hdg_comp[5];
  dtostrf(plane.hdg_comp, 4, 2, str_hdg_comp);
  addToBuff(buff, buffIndex, str_hdg_comp);

  char str_hdg_imu[5];
  dtostrf(plane.hdg_imu, 4, 2, str_hdg_imu);
  addToBuff(buff, buffIndex, str_hdg_imu);

  char str_pitch[5];
  dtostrf(plane.pitch, 4, 2, str_pitch);
  addToBuff(buff, buffIndex, str_pitch);

  char str_roll[5];
  dtostrf(plane.roll, 4, 2, str_roll);
  addToBuff(buff, buffIndex, str_roll);

  char str_pitchPulseLen[5];
  dtostrf(pitchPulseLen, 4, 0, str_pitchPulseLen);
  addToBuff(buff, buffIndex, str_pitchPulseLen);

  char str_rollPulseLen[5];
  dtostrf(rollPulseLen, 4, 0, str_rollPulseLen);
  addToBuff(buff, buffIndex, str_rollPulseLen);

  char str_yawPulseLen[5];
  dtostrf(yawPulseLen, 4, 0, str_yawPulseLen);
  addToBuff(buff, buffIndex, str_yawPulseLen);

  char str_throttlePulseLen[5];
  dtostrf(throttlePulseLen, 4, 0, str_throttlePulseLen);
  addToBuff(buff, buffIndex, str_throttlePulseLen, true); // Add newline

  for(int i=0; i<buffIndex; i++)
  {
    if(dataIndex >= 512) // Once an entire page is filled, save the data and reopen the file for more logging
    {
      myFile.close();

      if(!sd.exists(filename))
        setupLog();
      
      myFile = sd.open(filename, O_WRITE | O_APPEND);
      dataIndex = 0;
    }

    myFile.write(buff[i]);
    dataIndex++;
  }
}




void handleSD()
{
  if(useSD)
  {
    logData();
    myTerminal.handleCmds();
  }
}
