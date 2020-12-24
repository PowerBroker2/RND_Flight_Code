#include "IFC_Tools.h"




#define S0 22
#define S1 23
#define S2 40
#define S3 41

#define S4 2
#define S5 3
#define S6 4
#define S7 5

#define S8 6
#define S9 9
#define S10 30
#define S11 31

#define S12 32
#define S13 33
#define S14 36
#define S15 37

#define MUX_EN 27
#define MUX_S0 39
#define MUX_S1 26

#define CAM_1 0
#define CAM_2 1




IFC_Class IFC;

Servo nose;
Servo rollStab;
Servo pitchStab;
Servo pan;
Servo tilt;




void setup()
{
  IFC.begin();
  
  nose.attach(S4);
  rollStab.attach(S12);
  pitchStab.attach(S13);
  pan.attach(S14);
  tilt.attach(S15);

  IFC.controlInputs.pitch_command    = 1500;
  IFC.controlInputs.roll_command     = 1500;
  IFC.controlInputs.yaw_command      = 1500;
  IFC.controlInputs.throttle_command = 1500;

  setupMux();
  setChannel(CAM_1);
}




void loop()
{
  IFC.tick();

  if (IFC.linkConnected)
    IFC.updateServos();
  
  nose.writeMicroseconds(IFC.controlInputs.yaw_command);
  rollStab.writeMicroseconds(mapfloat(-IFC.telemetry.rollAngle, -90, 90, 500, 2500));
  pitchStab.writeMicroseconds(mapfloat(-IFC.telemetry.pitchAngle, -90, 90, 500, 2500));
  pan.writeMicroseconds(1500);
  tilt.writeMicroseconds(1800);
  
  IFC.sendTelem();
}




void setupMux()
{
  pinMode(MUX_EN, OUTPUT);
  pinMode(MUX_S0, OUTPUT);
  pinMode(MUX_S1, OUTPUT);
}




void setChannel(uint8_t channelNum)
{
  enableMux(true);
  
  digitalWrite(MUX_S0, bitRead(channelNum, 0));
  digitalWrite(MUX_S1, bitRead(channelNum, 1));
}




void enableMux(bool enable)
{
  digitalWrite(MUX_EN, !enable);
}
