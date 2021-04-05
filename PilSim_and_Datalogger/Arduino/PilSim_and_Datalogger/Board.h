#pragma once




#define FEEDBACK_PORT Serial1 // COM4
#define DEBUG_PORT    Serial  // COM10 if Serial2
#define TELEM_PORT    Serial3

#define ROTARY_BUTTON_PIN 21
#define DT  22
#define CLK 23

#define BUTTON_PIN 4

#define TSW_0_PIN 2
#define TSW_1_PIN 3




enum mode { PILSIM_ACTIVE, PILSIM_INACTIVE, LOG } boardMode;




void setupPorts()
{
  FEEDBACK_PORT.begin(115200);
  DEBUG_PORT.begin(115200);
  TELEM_PORT.begin(115200);
}




void setupBoard()
{
  setupPorts();
}
