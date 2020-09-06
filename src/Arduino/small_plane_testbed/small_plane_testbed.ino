#include "FireTimer.h"
#include "Autopilot.h"
#include "Board.h"
#include "Compass.h"
#include "Controls.h"
#include "GPS.h"
#include "IMU.h"
#include "Pitot.h"
#include "AutopilotControls.h"
#include "datalog.h"




#define DEBUG 1




void setup()
{
  Serial.begin(115200);
  
  setupIMU();
  setupCompass();
  setupGPS();
  setupServos();
  setupInterrupts();
  setupControllers();
  setupPitot();
  setupSD();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}




void loop()
{
  pollCompass();
  pollGPS();
  pollIMU();
  pollControls();
  pollPitot();
  
  handleControllers();
  handleSD();

  findLoopFreq();

#if DEBUG
  Serial.print("Loop Frequency: "); Serial.println(loopFreq);
  Serial.print("Lat: "); Serial.println(plane.lat, 10);
  Serial.print("Lon: "); Serial.println(plane.lon, 10);
  Serial.print("SOG: "); Serial.println(plane.sog, 10);
  Serial.print("COG: "); Serial.println(plane.cog, 10);
  Serial.print("Azm: "); Serial.println(azimuth);
  Serial.print("VaF: "); Serial.println(validFlags, BIN);
  Serial.print("CAI: "); Serial.println(courseAngleIMU);
  Serial.print("PiA: "); Serial.println(plane.pitch);
  Serial.print("RoA: "); Serial.println(plane.roll);
  Serial.print("PPL: "); Serial.println(pitchPulseLen);
  Serial.print("RPL: "); Serial.println(rollPulseLen);
  Serial.print("YPL: "); Serial.println(yawPulseLen);
  Serial.print("TPL: "); Serial.println(throttlePulseLen);
  Serial.print("IAS: "); Serial.println(plane.ias);
  Serial.println();
#endif
}
