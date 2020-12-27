#include "GS_Tools.h"
#include "Board.h"
#include "Controls.h"
#include "RS485.h"




GS_Class GS;
SerialTransfer rpiTransfer;




void setup()
{
  GS.begin();
  rpiTransfer.begin(GS_DEBUG_PORT);
  
  setupRS485();
  setupInterrupts();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}




void loop()
{
  if (GS.commandTimer.fire())
  {
    GS.controlInputs.roll_command     = I4_PulseLen;
    GS.controlInputs.pitch_command    = I2_PulseLen;
    GS.controlInputs.yaw_command      = I1_PulseLen;
    GS.controlInputs.throttle_command = I3_PulseLen;
    GS.sendCommands();
  }

  while(RS485_2.available())
    Serial.write(RS485_2.read());

  if (rpiTransfer.available())
  {
    //Do something
  }
}
