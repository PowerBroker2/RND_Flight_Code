#include "IFC_Tools.h"




const uint16_t TELEM_REPORT_PERIOD_MS = 20;
const int16_t  NWLG_OFFSET            = -100;




Servo nwlg;
Servo pitchStab;
Servo rollStab;




void setup()
{
  myIFC.begin();
  
  nwlg.attach(29);
  pitchStab.attach(27);
  rollStab.attach(28);

  nwlg.write(90);
  pitchStab.write(90);
  rollStab.write(90);
}




void loop()
{
  myIFC.grabData_GPS();
  myIFC.grabData_IMU();
  myIFC.grabData_Pitot();
  myIFC.updateServos();
  myIFC.sendTelem();
  
  nwlg.writeMicroseconds(constrain(map(myIFC.controlInputs.yaw_command, RUDDER_MIN, RUDDER_MAX, RUDDER_MAX, RUDDER_MIN) + NWLG_OFFSET, RUDDER_MIN, RUDDER_MAX));
}
