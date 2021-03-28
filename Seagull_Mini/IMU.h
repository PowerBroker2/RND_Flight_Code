#pragma once
#include "Arduino.h"
#include <Adafruit_BNO055.h>




FireTimer imuTimer;
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);




void setupIMU()
{
  imuTimer.begin(LIMITER_PERIOD);

  while (!bno.begin())
  {
    Serial.println("No BNO055 detected");
    delay(100);
  }
}




void pollIMU()
{
  //get IMU data and convert to degrees
  auto vect = bno.getQuat().toEuler();
  plane.hdg_imu = -vect.x() * (180 / M_PI);
  plane.pitch   =  vect.y() * (180 / M_PI);
  plane.roll    = -vect.z() * (180 / M_PI);

  if(plane.hdg_imu < 0)
    plane.hdg_imu = plane.hdg_imu + 360;

  if(isnan(plane.hdg_imu) || isnan(plane.pitch) || isnan(plane.roll))
  {
    validFlags &= ~(byte)IMU_VALID;

    if(imuTimer.fire(false))
      validFlags &= ~(byte)IMU_RECENT;
  }
  else
  {
    imuTimer.start();
    validFlags |= IMU_VALID;
    validFlags |= IMU_RECENT;
  }
}
