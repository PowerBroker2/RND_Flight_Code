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
  Serial.println("Loop_Frequency: "
                 "Engage_Autopilot: "
                 "Valid_Flags: "
                 "Altitude_GPS: "
                 "Lat "
                 "Lon "
                 "SOG_GPS "
                 "SOG_GPS_CALC "
                 "COG_GPS "
                 "COG_GPS_CALC "
                 "Heading_Compass "
                 "Heading_IMU "
                 "Pitch "
                 "Roll "
                 "Elevator_Pulse_Len "
                 "Aileron_Pulse_Len "
                 "Rudder_Pulse_Len "
                 "Throttle_Pulse_Len");
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
  Serial.print("VaF: "); Serial.println(validFlags, BIN);
  Serial.print("Alt: "); Serial.println(plane.alt_gps, 10);
  Serial.print("Lat: "); Serial.println(plane.lat, 10);
  Serial.print("Lon: "); Serial.println(plane.lon, 10);
  Serial.print("SOG: "); Serial.println(plane.ias_gps, 10);
  Serial.print("SGC: "); Serial.println(plane.ias_gps_calc, 10);
  Serial.print("COG: "); Serial.println(plane.cog_gps, 10);
  Serial.print("CGC: "); Serial.println(plane.cog_gps_calc, 10);
  Serial.print("Azm: "); Serial.println(plane.hdg_comp);
  Serial.print("CAI: "); Serial.println(plane.hdg_imu);
  Serial.print("PiA: "); Serial.println(plane.pitch);
  Serial.print("RoA: "); Serial.println(plane.roll);
  Serial.print("EPL: "); Serial.println(pitchPulseLen);
  Serial.print("APL: "); Serial.println(rollPulseLen);
  Serial.print("RPL: "); Serial.println(yawPulseLen);
  Serial.print("TPL: "); Serial.println(throttlePulseLen);
  Serial.println();
  
#elif DEBUG_PLOTTER
  Serial.print(loopFreq);               Serial.print(',');
  Serial.print(engageAP);               Serial.print(',');
  Serial.print(validFlags, BIN);        Serial.print(',');
  Serial.print(plane.alt_gps, 10);      Serial.print(',');
  Serial.print(plane.lat, 10);          Serial.print(',');
  Serial.print(plane.lon, 10);          Serial.print(',');
  Serial.print(plane.ias_gps, 10);      Serial.print(',');
  Serial.print(plane.ias_gps_calc, 10); Serial.print(',');
  Serial.print(plane.cog_gps, 10);      Serial.print(',');
  Serial.print(plane.cog_gps_calc, 10); Serial.print(',');
  Serial.print(plane.hdg_comp);         Serial.print(',');
  Serial.print(plane.hdg_imu);          Serial.print(',');
  Serial.print(plane.pitch);            Serial.print(',');
  Serial.print(plane.roll);             Serial.print(',');
  Serial.print(pitchPulseLen);          Serial.print(',');
  Serial.print(rollPulseLen);           Serial.print(',');
  Serial.print(yawPulseLen);            Serial.print(',');
  Serial.println(throttlePulseLen);
#endif
}
