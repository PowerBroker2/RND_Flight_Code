#include "IFC_Tools.h"
#include "FireTimer.h"




SerialTransfer telemTransfer;
FireTimer telemTimer;

const uint16_t TELEM_REPORT_PERIOD_MS = 20;
unsigned long  previous_time = millis();




void setup()
{
  myIFC.begin();
  Serial5.begin(2000000);

  telemTransfer.begin(Serial5);
  telemTimer.begin(TELEM_REPORT_PERIOD_MS);
}




void loop()
{
  myIFC.grabData_GPS();
  myIFC.grabData_IMU();
  myIFC.grabData_Pitot();
  myIFC.updateServos();

  if(telemTimer.fire())
  {
    previous_time += TELEM_REPORT_PERIOD_MS;
    sendToDatalogger();
  }
}




void sendToDatalogger()
{
  uint16_t sendLen;

  telemTransfer.txObj(myIFC.telemetry, sizeof(myIFC.telemetry));
  sendLen = sizeof(myIFC.telemetry);
  
  telemTransfer.txObj(myIFC.controlInputs, sizeof(myIFC.controlInputs), sendLen);
  sendLen += sizeof(myIFC.controlInputs);

  telemTransfer.sendData(sendLen);
}
