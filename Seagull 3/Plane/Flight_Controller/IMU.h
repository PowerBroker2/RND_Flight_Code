#pragma once
#include "Adafruit_BNO055.h"
#include "FireTimer.h"
#include "PacketStructs.h"




#define IMU_PORT Wire

#define COMPASS_RAD  vect.x()
#define PITCH_RAD    vect.y()
#define ROLL_RAD     vect.z()




const int IMU_PERIOD = 20; // ms

const int FLIP_COMPASS = 0;
const int FLIP_PITCH   = 0;
const int FLIP_ROLL    = 0;




Adafruit_BNO055 bno = Adafruit_BNO055(55);
FireTimer imuTimer;




void setupIMU()
{
  imuTimer.begin(IMU_PERIOD);
  
  if(!bno.begin())
  {
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  delay(1000);
    
  bno.setExtCrystalUse(true);
}




void pollIMU()
{
  // Get IMU data and convert to degrees
  auto vect = bno.getQuat().toEuler();

  telem.courseAngleIMU = COMPASS_RAD * (180 / M_PI);
  telem.pitchAngle     = PITCH_RAD   * (180 / M_PI);
  telem.rollAngle      = ROLL_RAD    * (180 / M_PI);

  if (FLIP_COMPASS)
    telem.courseAngleIMU *= -1;

  if (FLIP_PITCH)
    telem.pitchAngle *= -1;

  if (FLIP_ROLL)
    telem.rollAngle *= -1;

  // Timestamp the new data - regardless of where this function was called
  imuTimer.start();
}
