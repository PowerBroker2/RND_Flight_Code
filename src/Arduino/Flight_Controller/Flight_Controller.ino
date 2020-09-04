#include "IFC_Tools.h"




const uint16_t TELEM_REPORT_PERIOD_MS = 20;
const int16_t  NWLG_OFFSET            = -100;




FireTimer camAnim;
IFC_Class IFC;

Servo nwlg;
Servo rollStab;
Servo pitchStab;
Servo pan;
Servo tilt;

bool anim = true;




void setup()
{
  IFC.begin();

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
  IFC.tick();

  if (IFC.linkConnected)
    IFC.updateServos();
  
  nwlg.writeMicroseconds(constrain(map(IFC.controlInputs.yaw_command, RUDDER_MIN, RUDDER_MAX, RUDDER_MAX, RUDDER_MIN) + NWLG_OFFSET, RUDDER_MIN, RUDDER_MAX));
  pitchStab.writeMicroseconds(constrain(mapfloat(-IFC.telemetry.pitchAngle, -90, 90, 600, 2400) + 40, 1000, 2000));
  rollStab.writeMicroseconds(mapfloat(IFC.telemetry.rollAngle, -90, 90, 600, 2400) + 50);

  /*if (anim)
    animateCam();*/
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
