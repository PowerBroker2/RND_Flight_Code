#include "GS_Tools.h"




void setup()
{
  //initialize the core ground station software class
  myGS.begin();
}




void loop()
{                                      // Signal   | min | mid | max | directions
  GS_commandTransfer.txBuff[16] = 110; // FPV Pan  | 0   | 110 | 180 | left <-> right
  GS_commandTransfer.txBuff[17] = 170; // FPV Tilt | 80  | 170 | 180 | down <-> up
  GS_commandTransfer.txBuff[18] = 140; // Bay Door | 40  | n/a | 140 | open <-> closed
  
  //determine each command value based off GS sensor data and send commands to plane
  myGS.computeAndSendCommands();

  //send data to datalogging computer via debugging port
  myGS.sendTelem();
}
