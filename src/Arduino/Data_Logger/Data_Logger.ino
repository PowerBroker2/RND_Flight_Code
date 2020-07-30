#include "SerialTransfer.h"
#include "Shared_Tools.h"
#include "SdTerminal.h"




const char nameTemplate[] = "flight_%d.txt";
const char headerRow[] = "epoch_ms,alt_cm,course_angle_imu,roll_deg,pitch_deg,velocity_m/s,lat_dd,lon_dd,year,month,day,hour,min,sec,sog,cog,throttle_command,pitch_command,yaw_command,roll_command";
char target[] = "%d,%s,%s,%s,%s,%s,%d,%s,%s,%d,%d,%d,%d,%d,%s,%s,%s,%d,%d,%d,%d";




telemetry_struct telemetry;
control_inputs_struct controlInputs;

SerialTransfer telemTransfer;
SdFatSdioEX sd;
SdFile myFile;
Terminal myTerminal;

char filename[20];




void setup()
{
  // turn on power led
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  Serial.begin(115200);
  Serial1.begin(2000000);

  telemTransfer.begin(Serial1);
  
  setupLog();

  myTerminal.begin(sd);
}




void loop()
{
  if (telemTransfer.available())
    logData();

  myTerminal.handleCmds();
}



void setupLog()
{
  unsigned int flightCount = 1;

  while (!sd.begin())
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
}




void logData()
{
  uint16_t recLen = telemTransfer.rxObj(telemetry);
  recLen += telemTransfer.rxObj(controlInputs, recLen);

  char str_alt[15];
  char str_coAngIMU[15];
  char str_roll[15];
  char str_pitch[15];
  char str_vel[15];
  char str_lat[15];
  char str_lon[15];
  char str_sec[15];
  char str_sog[15];
  char str_cog[15];

  char buff[150];

  dtostrf(telemetry.altitude, 4, 2, str_alt);
  dtostrf(telemetry.courseAngleIMU, 4, 2, str_alt);
  dtostrf(telemetry.rollAngle, 4, 6, str_roll);
  dtostrf(telemetry.pitchAngle, 4, 6, str_pitch);
  dtostrf(telemetry.velocity, 4, 6, str_vel);
  dtostrf(telemetry.latitude, 4, 7, str_lat);
  dtostrf(telemetry.longitude, 4, 7, str_lon);
  dtostrf(telemetry.UTC_second, 4, 2, str_sec);
  dtostrf(telemetry.speedOverGround, 4, 2, str_sog);
  dtostrf(telemetry.courseOverGround, 4, 2, str_cog);

  sprintf(buff,
          target,
          millis(),
          str_alt,
          str_coAngIMU,
          str_roll,
          str_pitch,
          str_vel,
          telemetry.validFlags,
          str_lat,
          str_lon,
          telemetry.UTC_year,
          telemetry.UTC_month,
          telemetry.UTC_day,
          telemetry.UTC_hour,
          telemetry.UTC_minute,
          str_sec,
          str_sog,
          str_cog,
          controlInputs.throttle_command,
          controlInputs.pitch_command,
          controlInputs.yaw_command,
          controlInputs.roll_command);

  if (!sd.exists(filename))
    setupLog();

  myFile.open(filename, FILE_WRITE);
  myFile.println(buff);
  myFile.close();
}
