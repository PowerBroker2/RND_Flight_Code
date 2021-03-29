#pragma once
#include "Arduino.h"
#include "AutopilotControls.h"
#include "Board.h"




void setupPitot()
{
  analogReadResolution(16);
}




void pollPitot()
{
  plane.ias = analogRead(PITOT);
}
