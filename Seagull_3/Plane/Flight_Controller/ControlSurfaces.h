#pragma once
#include "Servo.h"
#include "Board.h"




#define USE_NOSEWHEEL 1



const int ESC_PIN = S0;
const int EL_PIN  = S3;
const int RUD_PIN = S4;
const int AIL_PIN = S2;

#if USE_NOSEWHEEL
const int NOSE_PIN = S1;
#endif

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

#if USE_NOSEWHEEL
const int NOSE_MAX = 2000;
const int NOSE_MIN = 1000;
const int NOSE_MID = (NOSE_MAX + NOSE_MIN) / 2;
#endif

const bool REVERSE_EL  = false;
const bool REVERSE_RUD = false;
const bool REVERSE_AIL = true;

#if USE_NOSEWHEEL
const bool REVERSE_NOSE = true;
#endif




Servo throttle;
Servo elevator;
Servo rudder;
Servo ailerons;

#if USE_NOSEWHEEL
Servo nosewheel;
#endif




int el_offset  = 0;
int rud_offset = 0;
int ail_offset = 0;

#if USE_NOSEWHEEL
int nose_offset = 50;
#endif




void setThrottle(const int& pWidth)
{
  throttle.writeMicroseconds(constrain(pWidth, ESC_MIN, ESC_MAX));
}




void setElevator(const int& pWidth)
{
  if (REVERSE_EL)
    elevator.writeMicroseconds(map(constrain(pWidth + el_offset, EL_MIN, EL_MAX), EL_MIN, EL_MAX, EL_MAX, EL_MIN));
  else
    elevator.writeMicroseconds(constrain(pWidth + el_offset, EL_MIN, EL_MAX));
}




void setRudder(const int& pWidth)
{
  if (REVERSE_RUD)
    rudder.writeMicroseconds(map(constrain(pWidth + rud_offset, RUD_MIN, RUD_MAX), RUD_MIN, RUD_MAX, RUD_MAX, RUD_MIN));
  else
    rudder.writeMicroseconds(constrain(pWidth + rud_offset, RUD_MIN, RUD_MAX));
}




void setAilerons(const int& pWidth)
{
  if (REVERSE_AIL)
    ailerons.writeMicroseconds(map(constrain(pWidth + ail_offset, AIL_MIN, AIL_MAX), AIL_MIN, AIL_MAX, AIL_MAX, AIL_MIN));
  else
    ailerons.writeMicroseconds(constrain(pWidth + ail_offset, AIL_MIN, AIL_MAX));
}




#if USE_NOSEWHEEL
void setNosewheel(const int& pWidth)
{
  if (REVERSE_NOSE)
    nosewheel.writeMicroseconds(map(constrain(pWidth + nose_offset, NOSE_MIN, NOSE_MAX), NOSE_MIN, NOSE_MAX, NOSE_MAX, NOSE_MIN));
  else
    nosewheel.writeMicroseconds(constrain(pWidth + nose_offset, NOSE_MIN, NOSE_MAX));
}
#endif




void setupSurfaces()
{
  throttle.attach(ESC_PIN);
  elevator.attach(EL_PIN);
  rudder.attach(RUD_PIN);
  ailerons.attach(AIL_PIN);

#if USE_NOSEWHEEL
  nosewheel.attach(NOSE_PIN);
#endif

  setThrottle(ESC_MIN);
  setElevator(EL_MID);
  setRudder(RUD_MID);
  setAilerons(AIL_MID);

#if USE_NOSEWHEEL
  setNosewheel(NOSE_MID);
#endif
}
