#pragma once
#include "Servo.h"




#define DEBUG_PORT Serial




const int DEBUG_BAUD = 115200;

const int SS0 = 14;
const int SS1 = 15;
const int SS2 = 16;
const int SS3 = 17;

const int SS4 = 18;
const int SS5 = 19;
const int SS6 = 20;
const int SS7 = 21;

const int SS8  = 22;
const int SS9  = 23;
const int SS10 = 24;
const int SS11 = 25;

const int SS12 = 26;
const int SS13 = 27;
const int SS14 = 38;
const int SS15 = 39;

const int IO_PINS[] = { SS0, SS1, SS2, SS3, SS4, SS5, SS6, SS7, SS8, SS9, SS10, SS11, SS12, SS13, SS14, SS15 };
const int AN_PINS[] = { SS0, SS1, SS2, SS3, SS4, SS5, SS6, SS7, SS8, SS9, SS14, SS15 };




Servo SS0Servo;
Servo SS1Servo;
Servo SS2Servo;
Servo SS3Servo;

Servo SS4Servo;
Servo SS5Servo;
Servo SS6Servo;
Servo SS7Servo;

Servo SS8Servo;
Servo SS9Servo;
Servo SS10Servo;
Servo SS11Servo;

Servo SS12Servo;
Servo SS13Servo;
Servo SS14Servo;
Servo SS15Servo;




void setupBoard()
{
  analogReadResolution(16);
  DEBUG_PORT.begin(DEBUG_BAUD);
}




Servo* findServo(const int& pin)
{
  Servo* servoPtr = NULL;
  
  switch (pin)
  {
    case SS0:
    {
      servoPtr = &SS0Servo;
      break;
    }
    
    case SS1:
    {
      servoPtr = &SS1Servo;
      break;
    }
    
    case SS2:
    {
      servoPtr = &SS2Servo;
      break;
    }
    
    case SS3:
    {
      servoPtr = &SS3Servo;
      break;
    }
    
    case SS4:
    {
      servoPtr = &SS4Servo;
      break;
    }
    
    case SS5:
    {
      servoPtr = &SS5Servo;
      break;
    }
    
    case SS6:
    {
      servoPtr = &SS6Servo;
      break;
    }
    
    case SS7:
    {
      servoPtr = &SS7Servo;
      break;
    }
    
    case SS8:
    {
      servoPtr = &SS8Servo;
      break;
    }
    
    case SS9:
    {
      servoPtr = &SS9Servo;
      break;
    }
    
    case SS10:
    {
      servoPtr = &SS10Servo;
      break;
    }
    
    case SS11:
    {
      servoPtr = &SS11Servo;
      break;
    }
    
    case SS12:
    {
      servoPtr = &SS12Servo;
      break;
    }
    
    case SS13:
    {
      servoPtr = &SS13Servo;
      break;
    }
    
    case SS14:
    {
      servoPtr = &SS14Servo;
      break;
    }
    
    case SS15:
    {
      servoPtr = &SS15Servo;
      break;
    }
  }

  return servoPtr;
}




void initServo(const int& pin)
{
  findServo(pin)->attach(pin);
}




void writeServo(const int& pin, const int& pWidth)
{
  findServo(pin)->writeMicroseconds(pWidth);
}




void detachServo(const int& pin)
{
  findServo(pin)->detach();
}
