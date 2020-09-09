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




#define DEBUG_MONITOR 0
#define DEBUG_PLOTTER 0




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

#if DEBUG_MONITOR
#elif DEBUG_PLOTTER
  Serial.println("Loop Frequency: "
                 "EAP: "
                 "Lat: "
                 "Lon: "
                 "SOG: "
                 "COG: "
                 "Azm: "
                 "VaF: "
                 "CAI: "
                 "PiA: "
                 "RoA: "
                 "PPL: "
                 "RPL: "
                 "YPL: "
                 "TPL: "
                 "IAS: ");
#endif
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

#if DEBUG_MONITOR
  Serial.print("Loop Frequency: "); Serial.println(loopFreq);
  Serial.print("EAP: "); Serial.println(engageAP);
  Serial.print("Alt: "); Serial.println(plane.alt, 10);
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
  
#elif DEBUG_PLOTTER
  Serial.print(loopFreq);         Serial.print(',');
  Serial.print(engageAP);         Serial.print(',');
  Serial.print(plane.lat, 10);    Serial.print(',');
  Serial.print(plane.lon, 10);    Serial.print(',');
  Serial.print(plane.sog, 10);    Serial.print(',');
  Serial.print(plane.cog, 10);    Serial.print(',');
  Serial.print(azimuth);          Serial.print(',');
  Serial.print(validFlags, BIN);  Serial.print(',');
  Serial.print(courseAngleIMU);   Serial.print(',');
  Serial.print(plane.pitch);      Serial.print(',');
  Serial.print(plane.roll);       Serial.print(',');
  Serial.print(pitchPulseLen);    Serial.print(',');
  Serial.print(rollPulseLen);     Serial.print(',');
  Serial.print(yawPulseLen);      Serial.print(',');
  Serial.print(throttlePulseLen); Serial.print(',');
  Serial.println(plane.ias);
#endif
}
