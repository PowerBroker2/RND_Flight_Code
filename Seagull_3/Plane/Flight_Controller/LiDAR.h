#pragma once
#include "SerialTransfer.h"




#define ALT_PORT Serial2




const int ALT_BAUD = 115200;




SerialTransfer altTransfer;




void setupLiDAR()
{
  ALT_PORT.begin(ALT_BAUD);
  altTransfer.begin(ALT_PORT);
}




void pollLiDAR()
{
  if (altTransfer.available())
  {
    uint16_t alt;
    altTransfer.rxObj(alt);
    telem.altitude = alt;
  }
}
