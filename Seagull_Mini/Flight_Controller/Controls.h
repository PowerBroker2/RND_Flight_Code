#pragma once
#include "Arduino.h"
#include "Servo.h"
#include "Board.h"




const int MS_HIGH   = 2000;
const int MS_MID    = 1500;
const int MS_LOW    = 1000;
const int ANGL_HIGH = 30;
const int ANGL_LOW  = -30;

volatile long I1_StartMicros = 0;
volatile long I1_PulseLen    = 0;
volatile bool I1_NewPulse    = 0;
volatile bool I1_HIGH        = 0;

volatile long I2_StartMicros = 0;
volatile long I2_PulseLen    = 0;
volatile bool I2_NewPulse    = 0;
volatile bool I2_HIGH        = 0;

volatile long I3_StartMicros = 0;
volatile long I3_PulseLen    = 0;
volatile bool I3_NewPulse    = 0;
volatile bool I3_HIGH        = 0;

volatile long I4_StartMicros = 0;
volatile long I4_PulseLen    = 0;
volatile bool I4_NewPulse    = 0;
volatile bool I4_HIGH        = 0;

volatile long I5_StartMicros = 0;
volatile long I5_PulseLen    = 0;
volatile bool I5_NewPulse    = 0;
volatile bool I5_HIGH        = 0;

volatile long I6_StartMicros = 0;
volatile long I6_PulseLen    = 0;
volatile bool I6_NewPulse    = 0;
volatile bool I6_HIGH        = 0;

volatile long I7_StartMicros = 0;
volatile long I7_PulseLen    = 0;
volatile bool I7_NewPulse    = 0;
volatile bool I7_HIGH        = 0;

volatile long I8_StartMicros = 0;
volatile long I8_PulseLen    = 0;
volatile bool I8_NewPulse    = 0;
volatile bool I8_HIGH        = 0;

const int PITCH_SERVO_PIN    = O7;
const int PITCH_INPUT_PIN    = I2;
long pitchPulseLen           = 0;
float pitchSetpoint          = 0;

const int ROLL_SERVO_PIN     = O5;
const int ROLL_INPUT_PIN     = I1;
long rollPulseLen            = 0;
float rollSetpoint           = 0;

const int YAW_SERVO_PIN      = O8;
const int YAW_INPUT_PIN      = I4;
long yawPulseLen             = 0;
float yawSetpoint            = 0;

const int THROTTLE_SERVO_PIN = O6;
const int THROTTLE_INPUT_PIN = I3;
long throttlePulseLen        = 0;

const int AP_INPUT_PIN = I5;
long apPulseLen        = 0;

extern bool engageAP;

const int FLAP_SERVO_PIN = O4;
const int FLAP_INPUT_PIN = I6;
long flapPulseLen        = 0;




Servo pitchServo;
Servo rollServo;
Servo yawServo;
Servo throttleServo;
Servo flapServo;




void setupServos()
{
  pitchServo.attach(PITCH_SERVO_PIN);
  rollServo.attach(ROLL_SERVO_PIN);
  yawServo.attach(YAW_SERVO_PIN);
  throttleServo.attach(THROTTLE_SERVO_PIN);
  flapServo.attach(FLAP_SERVO_PIN);
}




bool handleInput(int inputPin, long& pulseLen)
{
  if((inputPin == I1) && I1_NewPulse)
  {
    pulseLen = I1_PulseLen;
    return true;
  }
  else if((inputPin == I2) && I2_NewPulse)
  {
    pulseLen = I2_PulseLen;
    return true;
  }
  else if((inputPin == I3) && I3_NewPulse)
  {
    pulseLen = I3_PulseLen;
    return true;
  }
  else if((inputPin == I4) && I4_NewPulse)
  {
    pulseLen = I4_PulseLen;
    return true;
  }
  else if((inputPin == I5) && I5_NewPulse)
  {
    pulseLen = I5_PulseLen;
    return true;
  }
  else if((inputPin == I6) && I6_NewPulse)
  {
    pulseLen = I6_PulseLen;
    return true;
  }
  else if((inputPin == I7) && I7_NewPulse)
  {
    pulseLen = I7_PulseLen;
    return true;
  }
  else if((inputPin == I8) && I8_NewPulse)
  {
    pulseLen = I8_PulseLen;
    return true;
  }
  else
    return false;
}




void pollControls()
{
  if(handleInput(PITCH_INPUT_PIN, pitchPulseLen))
    pitchSetpoint = float_constrain(float_map(pitchPulseLen, MS_LOW, MS_HIGH, ANGL_LOW, ANGL_HIGH), ANGL_LOW, ANGL_HIGH);

  if(handleInput(ROLL_INPUT_PIN, rollPulseLen))
    rollSetpoint = float_constrain(float_map(rollPulseLen, MS_LOW, MS_HIGH, ANGL_LOW, ANGL_HIGH), ANGL_LOW, ANGL_HIGH);

  if(handleInput(YAW_INPUT_PIN, yawPulseLen))
    yawSetpoint = float_constrain(float_map(yawPulseLen, MS_LOW, MS_HIGH, ANGL_LOW, ANGL_HIGH), ANGL_LOW, ANGL_HIGH);

  if(handleInput(THROTTLE_INPUT_PIN, throttlePulseLen))
    throttleServo.write(throttlePulseLen);

  if(handleInput(AP_INPUT_PIN, apPulseLen))
  {
    if(apPulseLen > MS_MID)
    {
      validFlags &= ~(byte)MANUAL_CONTROL;
      engageAP = true;
    }
    else
    {
      validFlags |= MANUAL_CONTROL;
      engageAP = false;
    }
  }

  handleInput(FLAP_INPUT_PIN, flapPulseLen);
}




void I1_ISR()
{
  I1_HIGH = digitalRead(I1);
  
  if(I1_HIGH)
  {
    I1_StartMicros = micros();
    I1_NewPulse = false;
  }
  else
  {
    I1_PulseLen = micros() - I1_StartMicros;
    I1_NewPulse = true;
  }
}




void I2_ISR()
{
  I2_HIGH = digitalRead(I2);
  
  if(I2_HIGH)
  {
    I2_StartMicros = micros();
    I2_NewPulse = false;
  }
  else
  {
    I2_PulseLen = micros() - I2_StartMicros;
    I2_NewPulse = true;
  }
}




void I3_ISR()
{
  I3_HIGH = digitalRead(I3);
  
  if(I3_HIGH)
  {
    I3_StartMicros = micros();
    I3_NewPulse = false;
  }
  else
  {
    I3_PulseLen = micros() - I3_StartMicros;
    I3_NewPulse = true;
  }
}




void I4_ISR()
{
  I4_HIGH = digitalRead(I4);
  
  if(I4_HIGH)
  {
    I4_StartMicros = micros();
    I4_NewPulse = false;
  }
  else
  {
    I4_PulseLen = micros() - I4_StartMicros;
    I4_NewPulse = true;
  }
}




void I5_ISR()
{
  I5_HIGH = digitalRead(I5);
  
  if(I5_HIGH)
  {
    I5_StartMicros = micros();
    I5_NewPulse = false;
  }
  else
  {
    I5_PulseLen = micros() - I5_StartMicros;
    I5_NewPulse = true;
  }
}




void I6_ISR()
{
  I6_HIGH = digitalRead(I6);
  
  if(I6_HIGH)
  {
    I6_StartMicros = micros();
    I6_NewPulse = false;
  }
  else
  {
    I6_PulseLen = micros() - I6_StartMicros;
    I6_NewPulse = true;
  }
}




void I7_ISR()
{
  I7_HIGH = digitalRead(I7);
  
  if(I7_HIGH)
  {
    I7_StartMicros = micros();
    I7_NewPulse = false;
  }
  else
  {
    I7_PulseLen = micros() - I7_StartMicros;
    I7_NewPulse = true;
  }
}




void I8_ISR()
{
  I8_HIGH = digitalRead(I8);
  
  if(I8_HIGH)
  {
    I8_StartMicros = micros();
    I8_NewPulse = false;
  }
  else
  {
    I8_PulseLen = micros() - I8_StartMicros;
    I8_NewPulse = true;
  }
}




void setupInterrupts()
{
  pinMode(I1, INPUT);
  attachInterrupt(I1, I1_ISR, CHANGE);

  pinMode(I2, INPUT);
  attachInterrupt(I2, I2_ISR, CHANGE);
  
  pinMode(I3, INPUT);
  attachInterrupt(I3, I3_ISR, CHANGE);

  pinMode(I4, INPUT);
  attachInterrupt(I4, I4_ISR, CHANGE);

  pinMode(I5, INPUT);
  attachInterrupt(I5, I5_ISR, CHANGE);

  pinMode(I6, INPUT);
  attachInterrupt(I6, I6_ISR, CHANGE);

  pinMode(I7, INPUT);
  attachInterrupt(I7, I7_ISR, CHANGE);

  pinMode(I8, INPUT);
  attachInterrupt(I8, I8_ISR, CHANGE);
}
