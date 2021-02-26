#pragma once




#define I1 22
#define I2 21
#define I3 20
#define I4 19

#define I5 18
#define I6 17
#define I7 16
#define I8 15




unsigned long currentMicros;
unsigned long lastMicros;
float loopFreq;




void findLoopFreq()
{
  currentMicros = micros();
  loopFreq = 1000000.0 / (currentMicros - lastMicros);
  lastMicros = currentMicros;
}
