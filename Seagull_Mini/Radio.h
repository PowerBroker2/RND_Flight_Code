#pragma once
#include "Arduino.h"
#include "SerialTransfer.h"
#include "FireTimer.h"
#include "Board.h"




#define RAD_BAUD 115200
#define RAD_HZ   50




SerialTransfer telemTransfer;
FireTimer telemTimer;




void setupRadio()
{
  RAD_PORT.begin(RAD_BAUD);
  telemTransfer.begin(RAD_PORT);
  telemTimer.begin(1000 / RAD_HZ);
}




void sendTelem()
{
  if (telemTimer.fire())
  {
    int senLen = telemTransfer.txObj(plane);
    senLen = telemTransfer.txObj(engageAP, senLen);
    senLen = telemTransfer.txObj(validFlags, senLen);
    senLen = telemTransfer.txObj(pitchPulseLen, senLen);
    senLen = telemTransfer.txObj(rollPulseLen, senLen);
    senLen = telemTransfer.txObj(yawPulseLen, senLen);
    senLen = telemTransfer.txObj(throttlePulseLen, senLen);

    telemTransfer.sendData(senLen);
  }
}
