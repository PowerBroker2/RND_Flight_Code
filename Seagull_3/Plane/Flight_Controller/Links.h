#pragma once
#include "SerialTransfer.h"
#include "FireTimer.h"
#include "PacketStructs.h"




#define TELEM_PORT Serial1
#define C2_PORT    Serial3




const int C2_BAUD    = 115200;
const int TELEM_BAUD = 115200;

const int LOSS_LINK_TIMEOUT   = 1000; // ms
const int REPORT_TELEM_PERIOD = 100;  // ms




SerialTransfer commandTransfer;
SerialTransfer telemTransfer;
FireTimer lossLinkTimer;
FireTimer telemTimer;




void setupLinks()
{
  C2_PORT.begin(C2_BAUD);
  TELEM_PORT.begin(TELEM_BAUD);
  
  commandTransfer.begin(C2_PORT, false);
  telemTransfer.begin(TELEM_PORT, false);

  lossLinkTimer.begin(LOSS_LINK_TIMEOUT);
  telemTimer.begin(REPORT_TELEM_PERIOD);
}




bool linkFailover()
{
  if (lossLinkTimer.fire(false))
  {
    setThrottle(ESC_MIN);
    setElevator(EL_MID);
    setRudder(RUD_MID);
    setAilerons(AIL_MID);
    
    telem.validFlags = telem.validFlags & ~(byte)0x2;

    return true;
  }

  return false;
}




bool pollC2()
{
  if (commandTransfer.available())
  {
    commandTransfer.rxObj(inputs);

    setThrottle(inputs.throttle);
    setElevator(inputs.pitch);
    setRudder(inputs.yaw);
    setAilerons(inputs.roll);

#if USE_NOSEWHEEL
    setNosewheel(inputs.yaw);
#endif
    
    lossLinkTimer.start();
    telem.validFlags = telem.validFlags | 0x2;
  }

  return linkFailover();
}




void sendTelem(const int& extras = 0)
{
  if (telemTimer.fire())
  {
    uint16_t sendLen = telemTransfer.txObj(telem);
    sendLen = telemTransfer.txObj(inputs, sendLen);
    sendLen += extras;
    
    telemTransfer.sendData(sendLen);
  }
}
