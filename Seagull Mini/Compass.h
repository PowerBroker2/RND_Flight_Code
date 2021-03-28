#pragma once
#include "Arduino.h"
#include <QMC5883LCompass.h>
#include "AutopilotControls.h"




QMC5883LCompass compass;




void setupCompass()
{
  compass.init();
}




void pollCompass()
{
  compass.read();
  plane.hdg_comp = compass.getAzimuth();
}
