#pragma once
#include "Arduino.h"
#include "SD.h"
//#include "SdFat.h"
//#include "SdTerminal.h"




//SdFatSdioEX sd;
//Terminal myTerminal;




void setupSD()
{
  if (!SD.begin(BUILTIN_SDCARD))
    Serial.println ("NO LUCK");
  else
    Serial.println ("SD WORKS!");
    
  //sd.begin();
  //myTerminal.begin(sd);

  File dataFile = SD.open("datalog.txt", FILE_WRITE);
}




void handleSD()
{
  //myTerminal.handleCmds();
}
