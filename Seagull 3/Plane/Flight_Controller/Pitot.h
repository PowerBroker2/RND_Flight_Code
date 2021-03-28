#pragma once




const int PITOT_PIN = S6;




float V_0           = 3.3;   // supply voltage to the pressure sensor
float rho           = 1.204; // density of air
int offset          = 0;
int offset_size     = 10;
int veloc_mean_size = 20;
int zero_span       = 2;




void setupPitot()
{
  for (int i = 0; i < offset_size; i++)
    offset += analogRead(PITOT_PIN) - (65535 / 2);

  offset /= offset_size;
}




void pollPitot()
{
  float adc_avg = 0;
  float veloc   = 0;

  // average a few ADC readings for stability
  for (int i = 0; i < veloc_mean_size; i++)
    adc_avg += analogRead(PITOT_PIN) - offset;
  
  adc_avg /= veloc_mean_size;

  // make sure if the ADC reads below 512, then we equate it to a negative velocity
  if ((adc_avg > (512 - zero_span)) && (adc_avg < (512 + zero_span)))
  {
    
  }
  else
  {
    if (adc_avg < 512)
      veloc = -sqrt((-10000.0 * ((adc_avg / 65535.0) - 0.5)) / rho);
    else
      veloc = sqrt((10000.0 * ((adc_avg / 65535.0) - 0.5)) / rho);

    Serial.println(veloc);
  }
}
