#pragma once
#include "Arduino.h"
#include "AutopilotControls.h"
#include "Board.h"





const float RHO           = 1.2041; // Density of air at sea level and 20°C
const int OFFSET_SIZE     = 10;
const int VELOC_MEAN_SIZE = 10;
const int ZERO_SPAN       = 2;
const float ADC_MAX       = (float)(pow(2.0, ADC_RESOLUTION) - 1.0);
const float ADC_MID       = ADC_MAX / 2.0;




int offset = 0;




void setupPitot()
{
  int reading;
  
  for (int i = 0; i < OFFSET_SIZE; i++)
  {
    reading = analogRead(PITOT);
    offset += reading - ADC_MID;
  }

  offset /= OFFSET_SIZE;
}




void pollPitot()
{
  float adc_avg = 0;

  for (int i = 0; i < VELOC_MEAN_SIZE; i++)
    adc_avg += analogRead(PITOT) - offset;
  
  adc_avg /= VELOC_MEAN_SIZE;

  if ((adc_avg > (ADC_MID + ZERO_SPAN)) || (adc_avg < (ADC_MID - ZERO_SPAN)))
  {
    if (adc_avg < ADC_MID)
      plane.ias = -sqrt((-10000.0 * ((adc_avg / ADC_MAX) - 0.5)) / RHO);
    else
      plane.ias =  sqrt(( 10000.0 * ((adc_avg / ADC_MAX) - 0.5)) / RHO);
  }
}
