#pragma once
#include "Arduino.h"
#include <Adafruit_BNO055.h>




FireTimer imuTimer;
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);




float courseAngleIMU;




void setupIMU()
{
  imuTimer.begin(LIMITER_PERIOD);

  if (!bno.begin())
  {
    Serial.print("No BNO055 detected");
    while (1);
  }
}




void pollIMU()
{
  //get IMU data and convert to degrees
  auto vect = bno.getQuat().toEuler();
  courseAngleIMU  = vect.x() * (180 / M_PI);
  plane.pitch     = vect.z() * (180 / M_PI);
  plane.roll      = vect.y() * (180 / M_PI);

  //timestamp the new data - regardless of where this function was called
  imuTimer.start();
}
