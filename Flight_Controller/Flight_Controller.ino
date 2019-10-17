#include <SdFat.h>
#include <SerialTransfer.h>




SerialTransfer myTransfer;
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
  uint16_t throttle_command;
  uint16_t pitch_command;
  uint16_t yaw_command;
  uint16_t roll_command;
} telemetry;




void setup()
{
  // turn on power led
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  
  Serial.begin(115200);
  Serial3.begin(2000000);

  myTransfer.begin(Serial3);

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
  if(myTransfer.available() == NEW_DATA)
  {
    telemetry.velocity         = ((myTransfer.rxBuff[0]  << 8) | myTransfer.rxBuff[1])  / 100.0;
    telemetry.altitude         = ((myTransfer.rxBuff[2]  << 8) | myTransfer.rxBuff[3])  / 100.0;
    telemetry.pitchAngle       = ((myTransfer.rxBuff[4]  << 8) | myTransfer.rxBuff[5])  / 100.0;
    telemetry.rollAngle        = ((myTransfer.rxBuff[6]  << 8) | myTransfer.rxBuff[7])  / 100.0;
    telemetry.latitude         = ((myTransfer.rxBuff[8]  << 8) | myTransfer.rxBuff[9])  / 100.0;
    telemetry.longitude        = ((myTransfer.rxBuff[10] << 8) | myTransfer.rxBuff[11]) / 100.0;
    telemetry.UTC_year         =  (myTransfer.rxBuff[12] << 8) | myTransfer.rxBuff[13];
    telemetry.UTC_month        =   myTransfer.rxBuff[14];
    telemetry.UTC_day          =   myTransfer.rxBuff[15];
    telemetry.UTC_hour         =   myTransfer.rxBuff[16];
    telemetry.UTC_minute       =   myTransfer.rxBuff[17];
    telemetry.UTC_second       = ((myTransfer.rxBuff[18] << 8) | myTransfer.rxBuff[19]) / 100.0;
    telemetry.speedOverGround  = ((myTransfer.rxBuff[20] << 8) | myTransfer.rxBuff[21]) / 100.0;
    telemetry.courseOverGround = ((myTransfer.rxBuff[22] << 8) | myTransfer.rxBuff[23]) / 100.0;
    telemetry.throttle_command =  (myTransfer.rxBuff[24] << 8) | myTransfer.rxBuff[25];
    telemetry.pitch_command    =  (myTransfer.rxBuff[26] << 8) | myTransfer.rxBuff[27];
    telemetry.yaw_command      =  (myTransfer.rxBuff[28] << 8) | myTransfer.rxBuff[29];
    telemetry.roll_command     =  (myTransfer.rxBuff[30] << 8) | myTransfer.rxBuff[31];
    
    myFile.open(fileName, FILE_WRITE);

    myFile.print(millis());                      myFile.print(',');
    myFile.print(telemetry.altitude,   5);       myFile.print(',');
    myFile.print(telemetry.rollAngle,  5);       myFile.print(',');
    myFile.print(telemetry.pitchAngle, 5);       myFile.print(',');
    myFile.print(telemetry.velocity,   5);       myFile.print(',');
    myFile.print(telemetry.latitude,   5);       myFile.print(',');
    myFile.print(telemetry.longitude,  5);       myFile.print(',');
    myFile.print(telemetry.UTC_year);            myFile.print(',');
    myFile.print(telemetry.UTC_month);           myFile.print(',');
    myFile.print(telemetry.UTC_day);             myFile.print(',');
    myFile.print(telemetry.UTC_hour);            myFile.print(',');
    myFile.print(telemetry.UTC_minute);          myFile.print(',');
    myFile.print(telemetry.UTC_second);          myFile.print(',');
    myFile.print(telemetry.speedOverGround, 3);  myFile.print(',');
    myFile.print(telemetry.courseOverGround, 3); myFile.print(',');
    myFile.print(telemetry.throttle_command);    myFile.print(',');
    myFile.print(telemetry.pitch_command);       myFile.print(',');
    myFile.print(telemetry.yaw_command);         myFile.print(',');
    myFile.println(telemetry.roll_command);
    
    myFile.close();
  }
}
