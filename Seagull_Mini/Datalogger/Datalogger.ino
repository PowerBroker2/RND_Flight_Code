#include "SdFat.h"
#include "SerialTransfer.h"
#include "Autopilot.h"




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




const char nameTemplate[] = "flight_%d.csv";
const char headerRow[] = "Millis,"
                         "Engage_Autopilot,"
                         "Roll_Limit,"
                         "Roll_Limit_Max_Right,"
                         "Roll_Limit_Max_Left,"
                         "Pitch_Limit,"
                         "Pitch_Limit_Max_Up,"
                         "Pitch_Limit_Max_Down,"
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
                         "Throttle_Pulse_Len,"
                         "Flap_Pulse_Len";




SdFs sd;
FsFile myFile;
SerialTransfer telemTransfer;




state_params plane;
bool engageAP;
uint32_t _micros;
int MAX_ROLL_RIGHT;
int MAX_ROLL_LEFT;
bool limitRoll;
int MAX_PITCH_UP;
int MAX_PITCH_DOWN;
bool limitPitch;
uint8_t validFlags;
long pitchPulseLen;
long rollPulseLen;
long yawPulseLen;
long throttlePulseLen;
long flapPulseLen;

char filename[20];




void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);
  telemTransfer.begin(Serial1);
  
  setupLog();
}




void loop()
{
  if (telemTransfer.available())
  {
    int recLen = telemTransfer.rxObj(_micros);
    recLen = telemTransfer.rxObj(engageAP,         recLen);
    recLen = telemTransfer.rxObj(limitRoll,        recLen);
    recLen = telemTransfer.rxObj(MAX_ROLL_RIGHT,   recLen);
    recLen = telemTransfer.rxObj(MAX_ROLL_LEFT,    recLen);
    recLen = telemTransfer.rxObj(limitPitch,       recLen);
    recLen = telemTransfer.rxObj(MAX_PITCH_UP,     recLen);
    recLen = telemTransfer.rxObj(MAX_PITCH_DOWN,   recLen);
    recLen = telemTransfer.rxObj(validFlags,       recLen);
    recLen = telemTransfer.rxObj(plane,            recLen);
    recLen = telemTransfer.rxObj(pitchPulseLen,    recLen);
    recLen = telemTransfer.rxObj(rollPulseLen,     recLen);
    recLen = telemTransfer.rxObj(yawPulseLen,      recLen);
    recLen = telemTransfer.rxObj(throttlePulseLen, recLen);
    recLen = telemTransfer.rxObj(flapPulseLen,     recLen);
    
    logData();
  }
}




void setupLog()
{
  unsigned int flightCount = 1;

  if (!sd.begin(SD_CONFIG))
  {
    Serial.println(F("SD iniatialization failed"));
    delay(100);
  }

  sprintf(filename, "flight_%d.csv", flightCount);

  while (sd.exists(filename))
  {
    flightCount++;
    sprintf(filename, "flight_%d.csv", flightCount);
  }

  myFile = sd.open(filename, FILE_WRITE);
  myFile.println(headerRow);
  myFile.close();
  myFile = sd.open(filename, O_WRITE | O_APPEND);
}




void addToBuff(char buff[], int& colIndex, char data[], bool newLine = false)
{
  memcpy(buff + colIndex, data, strlen(data));
  colIndex += strlen(data);

  if (newLine)
    buff[colIndex] = '\n';
  else
    buff[colIndex] = ',';

  colIndex++;
}




template <typename T>
void addValue(char buff[], int& colIndex, const char& valBuffLen, const T& value, const char& width, const char& prec, bool newLine = false)
{
  char valBuff[valBuffLen];
  dtostrf(value, width, prec, valBuff);
  addToBuff(buff, colIndex, valBuff, newLine);
}




void writeOut(char buff[], int& colIndex)
{
  static int dataIndex = 0;
  
  for (int i = 0; i < colIndex; i++)
  {
    if (dataIndex >= 512) // Once an entire page is filled, save the data and reopen the file for more logging
    {
      myFile.close();

      if (!sd.exists(filename))
        setupLog();

      myFile = sd.open(filename, O_WRITE | O_APPEND);
      dataIndex = 0;
    }

    myFile.write(buff[i]);
    dataIndex++;
  }
}




void logData()
{
  char buff[1000];
  int colIndex = 0;

  addValue(buff, colIndex, 20, _micros,            10, 0);
  addValue(buff, colIndex, 5,  engageAP,           4,  0);
  addValue(buff, colIndex, 5,  limitRoll,          4,  0);
  addValue(buff, colIndex, 5,  MAX_ROLL_RIGHT,     4,  2);
  addValue(buff, colIndex, 5,  MAX_ROLL_LEFT,      4,  2);
  addValue(buff, colIndex, 5,  limitPitch,         4,  0);
  addValue(buff, colIndex, 5,  MAX_PITCH_UP,       4,  2);
  addValue(buff, colIndex, 5,  MAX_PITCH_DOWN,     4,  2);
  addValue(buff, colIndex, 5,  validFlags,         4,  0);
  addValue(buff, colIndex, 15, plane.alt_gps,      4,  10);
  addValue(buff, colIndex, 15, plane.lat,          4,  10);
  addValue(buff, colIndex, 15, plane.lon,          4,  10);
  addValue(buff, colIndex, 15, plane.ias_gps,      4,  10);
  addValue(buff, colIndex, 15, plane.ias_gps_calc, 4,  10);
  addValue(buff, colIndex, 15, plane.cog_gps,      4,  10);
  addValue(buff, colIndex, 15, plane.cog_gps_calc, 4,  10);
  addValue(buff, colIndex, 5,  plane.hdg_comp,     4,  2);
  addValue(buff, colIndex, 5,  plane.hdg_imu,      4,  2);
  addValue(buff, colIndex, 5,  plane.pitch,        4,  2);
  addValue(buff, colIndex, 5,  plane.roll,         4,  2);
  addValue(buff, colIndex, 5,  pitchPulseLen,      4,  0);
  addValue(buff, colIndex, 5,  rollPulseLen,       4,  0);
  addValue(buff, colIndex, 5,  yawPulseLen,        4,  0);
  addValue(buff, colIndex, 5,  throttlePulseLen,   4,  0);
  addValue(buff, colIndex, 5,  flapPulseLen,       4,  0, true); // add newline

  writeOut(buff, colIndex);
}
