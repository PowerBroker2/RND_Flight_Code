#pragma once
#include "Servo.h"
#include "Board.h"



const int ESC_PIN = S0;
const int EL_PIN  = S1;
const int RUD_PIN = S2;
const int AIL_PIN = S3;

const int ESC_MAX = 2000;
const int ESC_MIN = 1000;
const int ESC_MID = (ESC_MAX + ESC_MIN) / 2;

const int EL_MAX = 2000;
const int EL_MIN = 1000;
const int EL_MID = (EL_MAX + EL_MIN) / 2;

const int RUD_MAX = 2000;
const int RUD_MIN = 1000;
const int RUD_MID = (RUD_MAX + RUD_MIN) / 2;

const int AIL_MAX = 2000;
const int AIL_MIN = 1000;
const int AIL_MID = (AIL_MAX + AIL_MIN) / 2;




Servo throttle;
Servo elevator;
Servo rudder;
Servo ailerons;




void setThrottle(const int& pWidth)
{
  throttle.writeMicroseconds(constrain(pWidth, ESC_MIN, ESC_MAX));
}




void setElevator(const int& pWidth)
{
  elevator.writeMicroseconds(constrain(pWidth, EL_MIN, EL_MAX));
}




void setRudder(const int& pWidth)
{
  rudder.writeMicroseconds(constrain(pWidth, RUD_MIN, RUD_MAX));
}




void setAilerons(const int& pWidth)
{
  ailerons.writeMicroseconds(constrain(pWidth, AIL_MIN, AIL_MAX));
}




void setupSurfaces()
{
  throttle.attach(ESC_PIN);
  elevator.attach(EL_PIN);
  rudder.attach(RUD_PIN);
  ailerons.attach(AIL_PIN);

  setThrottle(ESC_MIN);
  setElevator(EL_MID);
  setRudder(RUD_MID);
  setAilerons(AIL_MID);
}
