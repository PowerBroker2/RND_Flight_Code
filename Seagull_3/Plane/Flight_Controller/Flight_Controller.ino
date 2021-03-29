#include "Watchdog_t4.h"
#include "Board.h"
#include "ControlSurfaces.h"
#include "GPS.h"
#include "IMU.h"
#include "LiDAR.h"
#include "Links.h"
#include "Payloads.h"
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
  setupPayloads();
  setupPitot();
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
  handlePayloads();
  pollPitot();
  pollSecondary();
}
