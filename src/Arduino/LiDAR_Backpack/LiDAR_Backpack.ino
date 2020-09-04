#include <LIDARLite.h>
#include <SerialTransfer.h>


LIDARLite myLidarLite;
SerialTransfer myTransfer;


void setup()
{
  Serial.begin(115200);
  myTransfer.begin(Serial);
  myLidarLite.begin(0, true);
  
  myLidarLite.configure(0);
}


void loop()
{
  sendDistance(myLidarLite.distance());
  
  for(int i = 0; i < 99; i++)
    sendDistance(myLidarLite.distance(false));
}


void sendDistance(const float &dist)
{
  myTransfer.txObj(dist);
  myTransfer.sendData(dist);
}
