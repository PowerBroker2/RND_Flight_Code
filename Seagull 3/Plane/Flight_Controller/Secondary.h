#pragma once
#include "SerialTransfer.h"
#include "PacketStructs.h"




#define SEC_PORT Serial5




const int SEC_BAUD = 115200;




SerialTransfer secTransfer;




void setupSecondary()
{
  SEC_PORT.begin(SEC_BAUD);
  secTransfer.begin(SEC_PORT);
}




void sendSecCommand()
{
  secTransfer.sendDatum(secInterface);
}




bool pollSecondary()
{
  if (secTransfer.available())
  {
    secTransfer.rxObj(secInterface);
    return true;
  }

  return false;
}
