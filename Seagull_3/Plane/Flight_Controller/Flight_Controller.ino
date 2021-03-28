#include "Watchdog_t4.h"
#include "Board.h"
#include "ControlSurfaces.h"
#include "GPS.h"
#include "IMU.h"
#include "LiDAR.h"
#include "Links.h"
#include "Pitot.h"
#include "Secondary.h"




void setup()
{
  setupBoard();
  setupSurfaces();
  setupGPS();
  setupIMU();
  setupLiDAR();
  setupLinks();
  setupSecondary();
}




void loop()
{
  resetWatchdog();
  pollGPS();
  pollIMU();
  pollLiDAR();
  pollC2();
  sendTelem();
  pollPitot();
  pollSecondary();
}
