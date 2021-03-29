#pragma once
#include "Adafruit_BNO055.h"
#include "FireTimer.h"
#include "PacketStructs.h"




#define IMU_PORT Wire

#define COMPASS_RAD  vect.z()
#define PITCH_RAD    vect.y()
#define ROLL_RAD     vect.x()




const int IMU_PERIOD = 20; // ms

const bool REVERSE_COMPASS = false;
const bool REVERSE_PITCH   = false;
const bool REVERSE_ROLL    = false;

const bool FLIP_COMPASS = false;
const bool FLIP_PITCH   = false;
const bool FLIP_ROLL    = true;




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
  

  if (isnan(telem.courseAngleIMU))
    telem.courseAngleIMU = 0;

  if (isnan(telem.pitchAngle))
    telem.pitchAngle = 0;
  
  if (isnan(telem.rollAngle))
    telem.rollAngle = 0;
    

  if (REVERSE_COMPASS)
    telem.courseAngleIMU *= -1;

  if (REVERSE_PITCH)
    telem.pitchAngle *= -1;

  if (REVERSE_ROLL)
    telem.rollAngle *= -1;
    

  if (FLIP_COMPASS)
    telem.courseAngleIMU = fmod(telem.courseAngleIMU + 180, 360);

  if (FLIP_PITCH)
    telem.pitchAngle = fmod(telem.pitchAngle + 180, 360);

  if (FLIP_ROLL)
    telem.rollAngle = fmod(telem.rollAngle + 180 - 23, 360); // WTF
    

  // Timestamp the new data - regardless of where this function was called
  imuTimer.start();
}
