#pragma once
#include "Servo.h"
#include "Board.h"




const int PITCH_STAB_PIN = S4;
const int ROLL_STAB_PIN  = S5;
const int PAN_PIN        = S10;
const int TILT_PIN       = S11;

const int PITCH_STAB_MAX = 2500;
const int PITCH_STAB_MIN = 500;
const int PITCH_STAB_MID = (PITCH_STAB_MAX + PITCH_STAB_MIN) / 2;

const int ROLL_STAB_MAX = 2500;
const int ROLL_STAB_MIN = 500;
const int ROLL_STAB_MID = (ROLL_STAB_MAX + ROLL_STAB_MIN) / 2;

const int PAN_MAX = 2500;
const int PAN_MIN = 500;
const int PAN_MID = (PAN_MAX + PAN_MIN) / 2;

const int TILT_MAX = 2500;
const int TILT_MIN = 500;
const int TILT_MID = (TILT_MAX + TILT_MIN) / 2;




Servo pitchStab;
Servo rollStab;
Servo pan;
Servo tilt;




void setPitchStab(const int& pWidth)
{
  pitchStab.writeMicroseconds(constrain(pWidth, PITCH_STAB_MIN, PITCH_STAB_MAX));
}




void setRollStab(const int& pWidth)
{
  rollStab.writeMicroseconds(constrain(pWidth, ROLL_STAB_MIN, ROLL_STAB_MAX));
}




void setPan(const int& pWidth)
{
  pan.writeMicroseconds(constrain(pWidth, PAN_MIN, PAN_MAX));
}




void setTilt(const int& pWidth)
{
  tilt.writeMicroseconds(constrain(pWidth, TILT_MIN, TILT_MAX));
}




void setupPayloads()
{
  pitchStab.attach(PITCH_STAB_PIN);
  rollStab.attach(ROLL_STAB_PIN);
  pan.attach(PAN_PIN);
  tilt.attach(TILT_PIN);

  setPitchStab(PITCH_STAB_MID);
  setRollStab(ROLL_STAB_MID);
  setPan(PAN_MID);
  setTilt(TILT_MID);
}





void handlePayloads()
{
  setPitchStab(map(telem.pitchAngle, -135, 135, PITCH_STAB_MIN, PITCH_STAB_MAX));
  setRollStab( map(telem.rollAngle,  -135, 135, ROLL_STAB_MIN,  ROLL_STAB_MAX));
  setPan(PAN_MID);
  setTilt(TILT_MID);
}
