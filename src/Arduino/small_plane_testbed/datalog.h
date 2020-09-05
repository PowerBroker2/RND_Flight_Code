#pragma once
#include "Arduino.h"
#include "SdFat.h"
#include "SdTerminal.h"




SdFatSdioEX sd;
Terminal myTerminal;




void setupSD()
{
  sd.begin();
  myTerminal.begin(sd);
}




void handleSD()
{
  myTerminal.handleCmds();
}
