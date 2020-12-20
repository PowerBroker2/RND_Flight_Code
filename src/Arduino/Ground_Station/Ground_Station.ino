#include "GS_Tools.h"
#include "Board.h"
#include "Controls.h"
#include "RS485.h"




GS_Class GS;




void setup()
{
  Serial.begin(115200);
  GS.begin();
  
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
}
