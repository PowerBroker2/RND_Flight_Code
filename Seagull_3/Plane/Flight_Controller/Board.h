#pragma once
#include "Watchdog_t4.h"




#define DEBUG_PORT Serial




const int DEBUG_BAUD = 115200;

const int ADC_RESOLUTION = 16;

const int MUX_S0 = 5;
const int MUX_S1 = 4;
const int MUX_S2 = 3;
const int MUX_S3 = 2;

const int S0 = 6;
const int S1 = 9;
const int S2 = 10;
const int S3 = 11;

const int S4 = 12;
const int S5 = 13;
const int S6 = 22;
const int S7 = 23;

const int S8  = 26;
const int S9  = 27;
const int S10 = 30;
const int S11 = 31;

const int S12 = 32;
const int S13 = 33;
const int S14 = 36;
const int S15 = 37;

const int S16 = 38;
const int S17 = 39;
const int S18 = 40;
const int S19 = 41;




WDT_T4<WDT1> wdt;




void wdWarning()
{
  Serial.println("FEED THE DOG SOON, OR RESET!");
}




void resetWatchdog()
{
  wdt.feed();
}




void selectCam(uint8_t camNum)
{
  digitalWrite(MUX_S0, bitRead(camNum, 0));
  digitalWrite(MUX_S1, bitRead(camNum, 1));
  digitalWrite(MUX_S2, bitRead(camNum, 2));
  digitalWrite(MUX_S3, bitRead(camNum, 3));
}




void setupMux()
{
  pinMode(MUX_S0, OUTPUT);
  pinMode(MUX_S1, OUTPUT);
  pinMode(MUX_S2, OUTPUT);
  pinMode(MUX_S3, OUTPUT);

  selectCam(0);
}




void setupPorts()
{
  DEBUG_PORT.begin(DEBUG_BAUD);
}




void setupWatchDog()
{
  WDT_timings_t config;
  config.trigger = 0.5; /* in seconds, 0->128 */
  config.timeout = 1;   /* in seconds, 0->128 */
  config.callback = wdWarning;
  wdt.begin(config);
}




void setupBoard()
{
  analogReadResolution(ADC_RESOLUTION);
  setupMux();
  setupPorts();
}
