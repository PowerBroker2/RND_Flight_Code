#include <SdFat.h>
#include <SerialTransfer.h>




SerialTransfer telemTransfer;
SdFatSdioEX sd;
SdFile myFile;

char fileName[] = {'t', 'e', 's', 't', 's', '.', 't', 'x', 't'};

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
  Serial3.begin(2000000);

  telemTransfer.begin(Serial3);

  while (!sd.begin())
  {
    Serial.println(F("\nSD iniatialization failed"));
    delay(100);
  }
  
  myFile.open(fileName, FILE_WRITE);
  myFile.println();
  myFile.println(F("millis, altitude, roll, pitch, velocity, lat, lon, year, month, day, hour, min, sec, sog, cog"));
  myFile.close();
}




void loop()
{
  if(telemTransfer.available() == NEW_DATA)
  {
    uint16_t recLen;
    
    telemTransfer.rxObj(telemetry, sizeof(telemetry));
    recLen = sizeof(telemetry);

    telemTransfer.txObj(controlInputs, sizeof(controlInputs), recLen);
    recLen += sizeof(controlInputs);
    
    myFile.open(fileName, FILE_WRITE);

    myFile.print(millis());                       myFile.print(',');
    myFile.print(telemetry.altitude,   5);        myFile.print(',');
    myFile.print(telemetry.rollAngle,  5);        myFile.print(',');
    myFile.print(telemetry.pitchAngle, 5);        myFile.print(',');
    myFile.print(telemetry.velocity,   5);        myFile.print(',');
    myFile.print(telemetry.latitude,   5);        myFile.print(',');
    myFile.print(telemetry.longitude,  5);        myFile.print(',');
    myFile.print(telemetry.UTC_year);             myFile.print(',');
    myFile.print(telemetry.UTC_month);            myFile.print(',');
    myFile.print(telemetry.UTC_day);              myFile.print(',');
    myFile.print(telemetry.UTC_hour);             myFile.print(',');
    myFile.print(telemetry.UTC_minute);           myFile.print(',');
    myFile.print(telemetry.UTC_second);           myFile.print(',');
    myFile.print(telemetry.speedOverGround, 3);   myFile.print(',');
    myFile.print(telemetry.courseOverGround, 3);  myFile.print(',');
    myFile.print(controlInputs.throttle_command); myFile.print(',');
    myFile.print(controlInputs.pitch_command);    myFile.print(',');
    myFile.print(controlInputs.yaw_command);      myFile.print(',');
    myFile.println(controlInputs.roll_command);
    
    myFile.close();
  }
}
