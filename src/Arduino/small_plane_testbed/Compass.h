#pragma once
#include "Arduino.h"
#include <QMC5883LCompass.h>




QMC5883LCompass compass;




float azimuth;




void setupCompass()
{
  compass.init();
}




void pollCompass()
{
  compass.read();
  azimuth = compass.getAzimuth();
}
