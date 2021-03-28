#include <LIDARLite.h>
#include <SerialTransfer.h>


LIDARLite myLidarLite;
SerialTransfer myTransfer;


const uint8_t DELAY = 10;


void setup()
{
  Serial.begin(115200);
  myTransfer.begin(Serial);
  myLidarLite.begin(0, true);
  
  myLidarLite.configure(0);
}


void loop()
{
  myTransfer.sendDatum((uint16_t)myLidarLite.distance());
  delay(DELAY);
  
  for(int i = 0; i < 99; i++)
  {
   myTransfer.sendDatum((uint16_t)myLidarLite.distance(false));
   delay(DELAY);
  }
}
