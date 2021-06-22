#pragma once
#include "SdFat.h"
#include "SerialTransfer.h"
#include "Autopilot.h"
#include "Board.h"
#include "Sim.h"




// Uncomment 'USE_EXTERNAL_SD' define to use an external SD card adapter or leave
// it commented to use the built in sd card.
#define USE_EXTERNAL_SD 

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

#define MAX_WP_ROWS 30
#define MAX_WP_COLS 30
#define MAX_VAL_LEN 20




const char nameTemplate[] = "flt_logs/flight_%d.csv";
const char headerRow[] = "Millis,"
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
SerialTransfer telemTransfer;
state_params planeTelem;

bool sdConnected;
bool wpFileSelected;
char wpFile[MAX_WP_ROWS][MAX_WP_COLS][MAX_VAL_LEN];
int wpfRow;
int wpfCol;
int numCols;
int numWpts;
char filename[20];
int dataIndex;

bool engageAP;
uint8_t validFlags;
long pitchPulseLen;
long rollPulseLen;
long yawPulseLen;
long throttlePulseLen;

extern nav_state navState;




bool setupSD()
{
  if (!sd.begin(SD_CONFIG))
    sdConnected = false;
  else
    sdConnected = true;

  return sdConnected;
}




bool setupLog()
{
  telemTransfer.begin(TELEM_PORT);

  return setupSD();
}




void setupLogFile()
{
  unsigned int flightCount = 1;
  
  sprintf(filename, nameTemplate, flightCount);

  while(sd.exists(filename))
  {
    flightCount++;
    sprintf(filename, nameTemplate, flightCount);
  }
  
  myFile = sd.open(filename, FILE_WRITE);
  myFile.println(headerRow);
  myFile.close();
  myFile = sd.open(filename, O_WRITE | O_APPEND);
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

  char str_millis[5];
  dtostrf(millis(), 4, 0, str_millis);
  addToBuff(buff, buffIndex, str_millis);

  char str_engageAP[5];
  dtostrf(engageAP, 4, 0, str_engageAP);
  addToBuff(buff, buffIndex, str_engageAP);

  char str_validFlags[5];
  dtostrf(validFlags, 4, 0, str_validFlags);
  addToBuff(buff, buffIndex, str_validFlags);
  
  char str_alt_gps[15];
  dtostrf(planeTelem.alt_gps, 4, 10, str_alt_gps);
  addToBuff(buff, buffIndex, str_alt_gps);

  char str_lat[15];
  dtostrf(planeTelem.lat, 4, 10, str_lat);
  addToBuff(buff, buffIndex, str_lat);

  char str_lon[15];
  dtostrf(planeTelem.lon, 4, 10, str_lon);
  addToBuff(buff, buffIndex, str_lon);

  char str_ias_gps[15];
  dtostrf(planeTelem.ias_gps, 4, 10, str_ias_gps);
  addToBuff(buff, buffIndex, str_ias_gps);

  char str_ias_gps_calc[15];
  dtostrf(planeTelem.ias_gps_calc, 4, 10, str_ias_gps_calc);
  addToBuff(buff, buffIndex, str_ias_gps_calc);

  char str_cog_gps[15];
  dtostrf(planeTelem.cog_gps, 4, 10, str_cog_gps);
  addToBuff(buff, buffIndex, str_cog_gps);

  char str_cog_gps_calc[15];
  dtostrf(planeTelem.cog_gps_calc, 4, 10, str_cog_gps_calc);
  addToBuff(buff, buffIndex, str_cog_gps_calc);

  char str_hdg_comp[5];
  dtostrf(planeTelem.hdg_comp, 4, 2, str_hdg_comp);
  addToBuff(buff, buffIndex, str_hdg_comp);

  char str_hdg_imu[5];
  dtostrf(planeTelem.hdg_imu, 4, 2, str_hdg_imu);
  addToBuff(buff, buffIndex, str_hdg_imu);

  char str_pitch[5];
  dtostrf(planeTelem.pitch, 4, 2, str_pitch);
  addToBuff(buff, buffIndex, str_pitch);

  char str_roll[5];
  dtostrf(planeTelem.roll, 4, 2, str_roll);
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




void logTick()
{
  static mode prevBoardMode = boardMode;

  if (!sdConnected)
  {    
    if (sd.begin(SD_CONFIG))
      sdConnected = true;
    else
      sdConnected = false;
  }
  
  if ((boardMode == LOG) && sdConnected)
  {
    if (boardMode != prevBoardMode)
      setupLogFile();
    
    if (telemTransfer.available())
    {
      int recLen = telemTransfer.rxObj(planeTelem);
      recLen = telemTransfer.rxObj(engageAP,         recLen);
      recLen = telemTransfer.rxObj(validFlags,       recLen);
      recLen = telemTransfer.rxObj(pitchPulseLen,    recLen);
      recLen = telemTransfer.rxObj(rollPulseLen,     recLen);
      recLen = telemTransfer.rxObj(yawPulseLen,      recLen);
      recLen = telemTransfer.rxObj(throttlePulseLen, recLen);

      logData();
    }
  }

  prevBoardMode = boardMode;
}




void loadWpFile(char* arr, int len)
{
  char* rowToken; 
  char* rowRest = arr;

  int row = 0;
  int col = 0;

  memset(wpFile, 0, sizeof(wpFile));
  
  while ((rowToken = strtok_r(rowRest, "\r\n", &rowRest))) 
  {
    char* colToken; 
    char* colRest = rowToken;
    
    while ((colToken = strtok_r(colRest, ",", &colRest)))
    {
      memcpy(wpFile[row][col], colToken, strlen(colToken));
      col++;
    }

    numCols = col;
    col = 0;
    row++;
  }

  wpfRow  = 1; // Skip header row
  wpfCol  = 0;
  numWpts = row;
  wpFileSelected = true;
  navState = TAKEOFF;
}
