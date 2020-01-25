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
  myIFC.grabData_LiDAR();
  myIFC.grabData_Pitot();
  myIFC.updateServos();
  //myIFC.sendTelem();

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

  Serial.print("Altitude (cm): "); Serial.println(myIFC.telemetry.altitude);
  Serial.print("Pitch Angle: ");   Serial.println(myIFC.telemetry.pitchAngle);
  Serial.print("Roll Angle: ");    Serial.println(myIFC.telemetry.rollAngle);
  Serial.print("Heading: ");       Serial.println(myIFC.telemetry.courseAngle);
  Serial.println();
}
