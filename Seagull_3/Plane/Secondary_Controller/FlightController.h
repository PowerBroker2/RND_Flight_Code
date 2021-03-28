#pragma once
#include "SerialTransfer.h"
#include "Log.h"
#include "PacketStructs.h"




#define TELEM_PORT Serial1
#define FC_PORT    Serial2




const int TELEM_BAUD = 115200;
const int FC_BAUD    = 115200;

const int DIGIAL_READ  = 0;
const int DIGIAL_WRITE = 1;
const int SERVO_INIT   = 2;
const int SERVO_WRITE  = 3;
const int SERVO_DETACH = 4;
const int ANALOG_READ  = 5;




SerialTransfer telemTransfer;
SerialTransfer fcTransfer;




bool inArr(const int& val, const int arr[], const int& size)
{
  for (int i = 0; i < size; i++)
    if (arr[i] == val)
      return true;
  
  return false;
}




void setupPorts()
{
  TELEM_PORT.begin(TELEM_BAUD);
  FC_PORT.begin(FC_BAUD);
  
  telemTransfer.begin(TELEM_PORT);
  fcTransfer.begin(FC_PORT);
}




void sendFcResponse()
{
  fcTransfer.sendDatum(fcInterface);
}




void handleFcRequest()
{
  if (inArr(fcInterface.pin, IO_PINS, sizeof(IO_PINS) / sizeof(int)))
  {
    if (fcInterface.type == ANALOG_READ)
    {
      if (inArr(fcInterface.pin, IO_PINS, sizeof(IO_PINS) / sizeof(int)))
      {
        fcInterface.error    = false;
        fcInterface.response = analogRead(fcInterface.pin);
        sendFcResponse();
      }
      else
      {
        fcInterface.error = true;
        sendFcResponse();
      }
    }
    else
    {
      fcInterface.error = false;
      
      switch (fcInterface.type)
      {
        case DIGIAL_READ:
        {
          pinMode(fcInterface.pin, INPUT_PULLUP);
          fcInterface.response = digitalRead(fcInterface.pin);
          sendFcResponse();
          break;
        }
        
        case DIGIAL_WRITE:
        {
          pinMode(fcInterface.pin, OUTPUT);
          digitalWrite(fcInterface.pin, fcInterface.command);
          break;
        }
        
        case SERVO_INIT:
        {
          initServo(fcInterface.pin);
          break;
        }
        
        case SERVO_WRITE:
        {
          writeServo(fcInterface.pin, fcInterface.command);
          break;
        }
        
        case SERVO_DETACH:
        {
          detachServo(fcInterface.pin);
          break;
        }
      }
    }
  }
  else
  {
    fcInterface.error = true;
    sendFcResponse();
  }
}



bool pollFC()
{
  if (fcTransfer.available())
  {
    fcTransfer.rxObj(fcInterface);
    handleFcRequest();

    return true;
  }

  return false;
}




bool pollTelem()
{
  if (telemTransfer.available())
  {
    uint16_t recLen = telemTransfer.rxObj(telem);
    recLen += telemTransfer.rxObj(inputs, recLen);
    
    logData();

    return true;
  }

  return false;
}
