#include "GS_Tools.h"




void setup()
{
  //initialize the core ground station software class
  myGS.begin();
}




void loop()
{
  //determine each command value based off GS sensor data and send commands to plane
  myGS.computeAndSendCommands();

  //send data to datalogging computer via debugging port
  myGS.sendTelem();
}
