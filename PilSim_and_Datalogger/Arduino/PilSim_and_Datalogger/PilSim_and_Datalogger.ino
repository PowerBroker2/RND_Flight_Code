#include "Board.h"
#include "Display.h"
#include "Interface.h"
#include "Log.h"
#include "Sim.h"




void setup()
{
  setupBoard();
  setupDisplay();
  setupInterface();
  setupLog();
  setupSim();
}




void loop()
{
  logTick();
  displayTick();
  interfaceTick();
}
