#include "Board.h"
#include "FlightController.h"
#include "Log.h"
#include "PacketStructs.h"




void setup()
{
  setupBoard();
  setupPorts();
  setupLog();
}




void loop()
{
  pollFC();
  pollTelem();
  handleTerminal();
}
