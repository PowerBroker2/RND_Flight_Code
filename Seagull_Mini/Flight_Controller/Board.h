#pragma once




#define GPS_PORT Serial1
#define RAD_PORT Serial2

#define I1 2
#define I2 3
#define I3 4
#define I4 5

#define I5 6
#define I6 9
#define I7 10
#define I8 11

#define O1 12
#define O2 24
#define O3 25
#define O4 26

#define O5 27
#define O6 28
#define O7 29
#define O8 30

#define PITOT A17

#define IMU_RECENT B100000
#define IMU_VALID   B10000
#define FIX_RECENT   B1000
#define FIX_VALID     B100
#define LINK_RECENT    B10
#define MANUAL_CONTROL  B1




uint8_t validFlags = MANUAL_CONTROL;
const int ADC_RESOLUTION = 16;




void setupBoard()
{
  analogReadResolution(ADC_RESOLUTION);
}
