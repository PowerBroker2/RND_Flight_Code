#pragma once
#include "SerialTransfer.h"




#define DEBUG_PORT Serial
#define TELEM_PORT Serial2




const int DEBUG_BAUD = 115200;
const int TELEM_BAUD = 115200;

const int I1 = 22;
const int I2 = 21;
const int I3 = 20;
const int I4 = 19;

const int I5 = 18;
const int I6 = 17;
const int I7 = 16;
const int I8 = 15;

const int C2_DIR_CTRL      = 2;
const int TELEM_DIR_CTRL_2 = 9;

const int TX = HIGH;
const int RX = LOW;




SerialTransfer rpiTransfer;




void setupBoard()
{
  pinMode(C2_DIR_CTRL,      OUTPUT);
  pinMode(TELEM_DIR_CTRL_2, OUTPUT);

  digitalWrite(C2_DIR_CTRL,      TX);
  digitalWrite(TELEM_DIR_CTRL_2, RX);

  DEBUG_PORT.begin(DEBUG_BAUD);
  TELEM_PORT.begin(TELEM_BAUD);

  rpiTransfer.begin(DEBUG_PORT);
}




void pollTelem()
{
  while(TELEM_PORT.available())
    DEBUG_PORT.write(TELEM_PORT.read());
}




void pollRpi()
{
  if (rpiTransfer.available())
  {
    //Do something
  }
}
