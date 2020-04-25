#include <SdFat.h>
#include <SerialTransfer.h>




SerialTransfer telemTransfer;
SdFatSdioEX sd;
SdFile myFile;

char filename[20];

struct telemetry
{
  float altitude;         //cm
  float rollAngle;        //radians
  float pitchAngle;       //radians
  float velocity;         //m/s
  float latitude;         //dd
  float longitude;        //dd
  uint16_t UTC_year;      //y
  uint8_t UTC_month;      //M
  uint8_t UTC_day;        //d
  uint8_t UTC_hour;       //h
  uint8_t UTC_minute;     //m
  float UTC_second;       //s
  float speedOverGround;  //knots
  float courseOverGround; //degrees
} telemetry;

struct controlInputs
{
  bool limiter_enable; //enables and disables pitch and bank limiter
  uint16_t pitch_command;
  uint16_t roll_command;
  uint16_t yaw_command;
  uint16_t throttle_command;
  uint16_t autopilot_command;
  uint16_t limiter_command;
  uint16_t gear_command;
  uint16_t flaps_command;
} controlInputs;




void setup()
{
  // turn on power led
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  
  Serial.begin(115200);
  Serial1.begin(2000000);

  telemTransfer.begin(Serial1);

  while (!sd.begin())
  {
    Serial.println(F("\nSD iniatialization failed"));
    delay(100);
  }
  
  setupSD();
}




void loop()
{
  if(telemTransfer.available())
  {
    uint16_t recLen;
    
    telemTransfer.rxObj(telemetry, sizeof(telemetry));
    recLen = sizeof(telemetry);

    telemTransfer.txObj(controlInputs, sizeof(controlInputs), recLen);
    recLen += sizeof(controlInputs);
    
    logData();
  }
  else if (telemTransfer.status < 0)
  {
    Serial.print(F("ERROR: "));
  
    if(telemTransfer.status == -1)
      Serial.println(F("CRC_ERROR"));
    else if(telemTransfer.status == -2)
      Serial.println(F("PAYLOAD_ERROR"));
    else if(telemTransfer.status == -3)
      Serial.println(F("STOP_BYTE_ERROR"));
  }
}




void setupSD()
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
  myFile.println(F("millis, altitude, roll, pitch, velocity, lat, lon, year, month, day, hour, min, sec, sog, cog, throttle command, yaw command, roll command"));
  myFile.close();
}




void logData()
{
  char str_temp[6];
  char buff[100];
  char target[] = "%d, %s, %s, %s, %s, %s, %s, %d, %d, %d, %d, %d, %s, %s, %s, %d, %d, %d, %d";

  sprintf(buff, target, millis(),
                        dtostrf(telemetry.altitude, 4, 2, str_temp),
                        dtostrf(telemetry.rollAngle, 4, 2, str_temp),
                        dtostrf(telemetry.pitchAngle, 4, 2, str_temp),
                        dtostrf(telemetry.velocity, 4, 2, str_temp),
                        dtostrf(telemetry.latitude, 4, 2, str_temp),
                        dtostrf(telemetry.longitude, 4, 2, str_temp),
                        telemetry.UTC_year,
                        telemetry.UTC_month,
                        telemetry.UTC_day,
                        telemetry.UTC_hour,
                        telemetry.UTC_minute,
                        dtostrf(telemetry.UTC_second, 4, 2, str_temp),
                        dtostrf(telemetry.speedOverGround, 4, 2, str_temp),
                        dtostrf(telemetry.courseOverGround, 4, 2, str_temp),
                        controlInputs.throttle_command,
                        controlInputs.pitch_command,
                        controlInputs.yaw_command,
                        controlInputs.roll_command);
  
  myFile.open(filename, FILE_WRITE);
  myFile.println(buff);
  myFile.close();
}
