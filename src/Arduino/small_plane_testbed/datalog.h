#pragma once
#include "Arduino.h"
#include "SdFat.h" // SdFat.h Ver 2.0.0 Beta
//#include "SdTerminal.h"
#include "IMU.h"
#include "Controls.h"
#include "Board.h"




const char nameTemplate[] = "flight_%d.csv";
const char headerRow[] = "epoch_ms,"
                         "alt_m,"
                         "course_angle_imu,"
                         "roll_deg,"
                         "pitch_deg,"
                         "pitot_pressure,"
                         "valid_flags,"
                         "lat_dd,"
                         "lon_dd,"
                         "sog,"
                         "cog,"
                         "throttle_command,"
                         "pitch_command,"
                         "yaw_command,"
                         "roll_command";




SdExFat sd;
//Terminal myTerminal;




bool useSD = true;
char filename[20];




void setupLog()
{
  unsigned int flightCount = 1;
  
  if(!sd.begin(SdioConfig(FIFO_SDIO)))
  {
    useSD = false;
    Serial.println(F("SD iniatialization failed"));
    delay(100);
  }

  if(useSD)
  {
    sprintf(filename, "flight_%d.csv", flightCount);
  
    while(sd.exists(filename))
    {
      flightCount++;
      sprintf(filename, "flight_%d.csv", flightCount);
    }
    
    auto myFile = sd.open(filename, FILE_WRITE);
    myFile.println(headerRow);
    myFile.close();
  }
}




void setupSD()
{
  if(useSD)
  {
    setupLog();
    //myTerminal.begin(sd);
  }
}




void logData()
{
  if (!sd.exists(filename))
    setupLog();

  auto myFile = sd.open(filename, O_WRITE | O_APPEND);
  
  myFile.print(millis());           myFile.print(',');
  myFile.print(plane.alt, 10);      myFile.print(',');
  myFile.print(courseAngleIMU, 10); myFile.print(',');
  myFile.print(plane.roll, 10);     myFile.print(',');
  myFile.print(plane.pitch, 10);    myFile.print(',');
  myFile.print(plane.ias);          myFile.print(',');
  myFile.print(validFlags, BIN);    myFile.print(',');
  myFile.print(plane.lat, 10);      myFile.print(',');
  myFile.print(plane.lon, 10);      myFile.print(',');
  myFile.print(plane.sog, 10);      myFile.print(',');
  myFile.print(plane.cog, 10);      myFile.print(',');
  myFile.print(throttlePulseLen);   myFile.print(',');
  myFile.print(pitchPulseLen);      myFile.print(',');
  myFile.print(yawPulseLen);        myFile.print(',');
  myFile.println(rollPulseLen);
  
  myFile.close();
}




void handleSD()
{
  if(useSD)
  {
    logData();
    //myTerminal.handleCmds();
  }
}
