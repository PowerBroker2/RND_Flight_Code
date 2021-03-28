#include "Board.h"
#include "Controls.h"




void setup()
{
  setupBoard();
  setupControls();
}




void loop()
{
  pollTelem();
  pollRpi();
  sendCommands();
}
