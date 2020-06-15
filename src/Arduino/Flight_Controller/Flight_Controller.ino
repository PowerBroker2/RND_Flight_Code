#include "IFC_Tools.h"




const uint16_t TELEM_REPORT_PERIOD_MS = 20;
const int16_t  NWLG_OFFSET            = -100;




FireTimer camAnim;

Servo nwlg;
Servo rollStab;
Servo pitchStab;
Servo pan;
Servo tilt;

bool anim = true;




void setup()
{
  myIFC.begin();

  camAnim.begin(10);
  
  nwlg.attach(29);
  rollStab.attach(28);
  pitchStab.attach(27);
  pan.attach(25);
  tilt.attach(26);

  nwlg.write(90);
  pitchStab.write(90);
  rollStab.write(90);
  pan.write(90);
  tilt.write(90);
}




void loop()
{
  myIFC.grabData_GPS();
  myIFC.grabData_IMU();
  myIFC.grabData_Pitot();
  myIFC.sendTelem();
  
  if(myIFC.handleSerialEvents())
    myIFC.updateServos();
  
  nwlg.writeMicroseconds(constrain(map(myIFC.controlInputs.yaw_command, RUDDER_MIN, RUDDER_MAX, RUDDER_MAX, RUDDER_MIN) + NWLG_OFFSET, RUDDER_MIN, RUDDER_MAX));
  pitchStab.writeMicroseconds(constrain(mapfloat(-myIFC.telemetry.pitchAngle, -90, 90, 600, 2400) + 40, 1000, 2000));
  rollStab.writeMicroseconds(mapfloat(myIFC.telemetry.rollAngle, -90, 90, 600, 2400) + 50);

  if (anim)
    animateCam();
}




void animateCam()
{
  static uint16_t panPos = 1000;
  static uint16_t tiltPos = 1000;
  static bool dir = true;

  if (camAnim.fire())
  {
    if (dir)
    {
      if ((panPos <= 2000) && (tiltPos <= 2000))
      {
        pan.writeMicroseconds(panPos);
        tilt.writeMicroseconds(tiltPos);
    
        panPos++;
        tiltPos++;
      }
      else
        dir = false;
    }
    else
    {
      if ((panPos >= 1500) && (tiltPos >= 1500))
      {
        pan.writeMicroseconds(panPos);
        tilt.writeMicroseconds(tiltPos);
    
        panPos--;
        tiltPos--;
      }
      else
        anim = false;
    }
  }
}




float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
