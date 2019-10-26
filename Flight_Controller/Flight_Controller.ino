#include "IFC_Tools.h"




SerialTransfer telemTx;




void setup()
{
  myIFC.begin();
  Serial5.begin(2000000);

  telemTx.begin(Serial5);
}




void loop()
{
  myIFC.grabData_GPS();
  myIFC.grabData_IMU();
  myIFC.grabData_LiDAR();
  myIFC.grabData_Pitot();
  myIFC.updateServos();
  //myIFC.sendTelem();

  sendToDatalogger();
}




void sendToDatalogger()
{
  telemTx.txBuff[0]  = (uint8_t)(((int16_t)(myIFC.telemetry.velocity   * 100)) >> 8);
  telemTx.txBuff[1]  = (uint8_t)(((int16_t)(myIFC.telemetry.velocity   * 100)) & 0xFF);
  telemTx.txBuff[2]  = (uint8_t)(((int16_t)(myIFC.telemetry.altitude   * 100)) >> 8);
  telemTx.txBuff[3]  = (uint8_t)(((int16_t)(myIFC.telemetry.altitude   * 100)) & 0xFF);
  telemTx.txBuff[4]  = (uint8_t)(((int16_t)(myIFC.telemetry.pitchAngle * 100)) >> 8);
  telemTx.txBuff[5]  = (uint8_t)(((int16_t)(myIFC.telemetry.pitchAngle * 100)) & 0xFF);
  telemTx.txBuff[6]  = (uint8_t)(((int16_t)(myIFC.telemetry.rollAngle  * 100)) >> 8);
  telemTx.txBuff[7]  = (uint8_t)(((int16_t)(myIFC.telemetry.rollAngle  * 100)) & 0xFF);
  telemTx.txBuff[8]  = (uint8_t)(((int16_t)(myIFC.telemetry.latitude   * 100)) >> 8);
  telemTx.txBuff[9]  = (uint8_t)(((int16_t)(myIFC.telemetry.latitude   * 100)) & 0xFF);
  telemTx.txBuff[10] = (uint8_t)(((int16_t)(myIFC.telemetry.longitude  * 100)) >> 8);
  telemTx.txBuff[11] = (uint8_t)(((int16_t)(myIFC.telemetry.longitude  * 100)) & 0xFF);
  telemTx.txBuff[12] = (uint8_t)(myIFC.telemetry.UTC_year >> 8);
  telemTx.txBuff[13] = (uint8_t)(myIFC.telemetry.UTC_year & 0xFF);
  telemTx.txBuff[14] = myIFC.telemetry.UTC_month;
  telemTx.txBuff[15] = myIFC.telemetry.UTC_day;
  telemTx.txBuff[16] = myIFC.telemetry.UTC_hour;
  telemTx.txBuff[17] = myIFC.telemetry.UTC_minute;
  telemTx.txBuff[18] = (uint8_t)(((int16_t)(myIFC.telemetry.UTC_second * 100)) >> 8);
  telemTx.txBuff[19] = (uint8_t)(((int16_t)(myIFC.telemetry.UTC_second * 100)) & 0xFF);
  telemTx.txBuff[20] = (uint8_t)(((int16_t)(myIFC.telemetry.speedOverGround  * 100)) >> 8);
  telemTx.txBuff[21] = (uint8_t)(((int16_t)(myIFC.telemetry.speedOverGround  * 100)) & 0xFF);
  telemTx.txBuff[22] = (uint8_t)(((int16_t)(myIFC.telemetry.courseOverGround * 100)) >> 8);
  telemTx.txBuff[23] = (uint8_t)(((int16_t)(myIFC.telemetry.courseOverGround * 100)) & 0xFF);
  telemTx.txBuff[24] = (uint8_t)(myIFC.controlInputs.throttle_command >> 8);
  telemTx.txBuff[25] = (uint8_t)(myIFC.controlInputs.throttle_command & 0xFF);
  telemTx.txBuff[26] = (uint8_t)(myIFC.controlInputs.pitch_command    >> 8);
  telemTx.txBuff[27] = (uint8_t)(myIFC.controlInputs.pitch_command    & 0xFF);
  telemTx.txBuff[28] = (uint8_t)(myIFC.controlInputs.yaw_command      >> 8);
  telemTx.txBuff[29] = (uint8_t)(myIFC.controlInputs.yaw_command      & 0xFF);
  telemTx.txBuff[30] = (uint8_t)(myIFC.controlInputs.roll_command     >> 8);
  telemTx.txBuff[31] = (uint8_t)(myIFC.controlInputs.roll_command     & 0xFF);

  telemTx.sendData(32);
}
