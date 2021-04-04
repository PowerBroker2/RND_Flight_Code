#pragma once
#include "OneButton.h"
#include "Board.h"
#include "Sim.h"




OneButton btn = OneButton(
  BUTTON_PIN, // Input pin for the button
  true        // Button is active LOW
);




bool tsw_0_cur  = true;
bool tsw_1_cur  = true;

bool tsw_0_prev = true;
bool tsw_1_prev = true;




static void handleClick()
{
  pitchController.reset();
  rollController.reset();
  headingController.reset();
  altitudeController.reset();
  iasController.reset();
}




void setupInterface()
{
  btn.attachClick(handleClick);

  pinMode(TSW_0_PIN, INPUT_PULLUP);
  pinMode(TSW_1_PIN, INPUT_PULLUP);

  tsw_0_cur = digitalRead(TSW_0_PIN);
  tsw_1_cur = digitalRead(TSW_1_PIN);

  if (tsw_0_cur && tsw_1_cur) // middle
    boardMode = PILSIM_INACTIVE;
  else if (tsw_0_cur && !tsw_1_cur) // down
    boardMode = LOG;
  else if (!tsw_0_cur && tsw_1_cur) // up
    boardMode = PILSIM_ACTIVE;
}




void interfaceTick()
{
  btn.tick();
  
  tsw_0_cur = digitalRead(TSW_0_PIN);
  tsw_1_cur = digitalRead(TSW_1_PIN);
  
  if ((tsw_0_cur != tsw_0_prev) || (tsw_1_cur != tsw_1_prev))
  {
    if (tsw_0_cur && tsw_1_cur) // middle
      boardMode = PILSIM_INACTIVE;
    else if (tsw_0_cur && !tsw_1_cur) // down
      boardMode = LOG;
    else if (!tsw_0_cur && tsw_1_cur) // up
      boardMode = PILSIM_ACTIVE;
  }

  tsw_0_prev = tsw_0_cur;
  tsw_1_prev = tsw_1_cur;
}
