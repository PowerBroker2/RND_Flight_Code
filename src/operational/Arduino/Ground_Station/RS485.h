#pragma once




#define RS485_1 Serial1 //TX
#define RS485_2 Serial2 //RX

#define DIR_CTRL_1 2
#define DIR_CTRL_2 9

#define TX HIGH
#define RX LOW




void setupRS485()
{
  pinMode(DIR_CTRL_1, OUTPUT);
  pinMode(DIR_CTRL_2, OUTPUT);

  digitalWrite(DIR_CTRL_1, TX);
  digitalWrite(DIR_CTRL_2, RX);
}
