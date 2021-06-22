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
    int senLen = telemTransfer.txObj((uint32_t)micros());
    senLen = telemTransfer.txObj(engageAP,         senLen);
    senLen = telemTransfer.txObj(limitRoll,        senLen);
    senLen = telemTransfer.txObj(MAX_ROLL_RIGHT,   senLen);
    senLen = telemTransfer.txObj(MAX_ROLL_LEFT,    senLen);
    senLen = telemTransfer.txObj(limitPitch,       senLen);
    senLen = telemTransfer.txObj(MAX_PITCH_UP,     senLen);
    senLen = telemTransfer.txObj(MAX_PITCH_DOWN,   senLen);
    senLen = telemTransfer.txObj(validFlags,       senLen);
    senLen = telemTransfer.txObj(plane,            senLen);
    senLen = telemTransfer.txObj(pitchPulseLen,    senLen);
    senLen = telemTransfer.txObj(rollPulseLen,     senLen);
    senLen = telemTransfer.txObj(yawPulseLen,      senLen);
    senLen = telemTransfer.txObj(throttlePulseLen, senLen);
    senLen = telemTransfer.txObj(flapPulseLen,     senLen);

    telemTransfer.sendData(senLen);
  }
}
