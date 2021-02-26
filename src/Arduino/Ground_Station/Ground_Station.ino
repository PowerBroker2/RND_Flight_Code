#include "GS_Tools.h"
#include "Board.h"
#include "Controls.h"
#include "RS485.h"




GS_Class GS;
SerialTransfer rpiTransfer;




struct __attribute__((__packed__)) extra_channels_struct
{
  uint16_t ec1;
  uint16_t ec2;
  uint16_t ec3;
  uint16_t ec4;
} extraChannels;




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
    GS.controlInputs.yaw_command      = I1_PulseLen;
    GS.controlInputs.pitch_command    = reverse(I2_PulseLen);
    GS.controlInputs.throttle_command = I3_PulseLen;
    GS.controlInputs.roll_command     = I4_PulseLen;
    
    extraChannels.ec1 = I5_PulseLen;
    extraChannels.ec2 = I6_PulseLen;
    extraChannels.ec3 = I7_PulseLen;
    extraChannels.ec4 = I8_PulseLen;

    GS.commandTransfer.txObj(extraChannels, sizeof(GS.controlInputs));
    GS.sendCommands();
  }

  while(RS485_2.available())
    Serial.write(RS485_2.read());

  if (rpiTransfer.available())
  {
    //Do something
  }
}
