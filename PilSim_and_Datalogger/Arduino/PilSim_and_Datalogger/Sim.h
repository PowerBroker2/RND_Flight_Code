#pragma once
#include "SerialTransfer.h"
#include "Autopilot.h"
#include "Log.h"




const int JOY_MAX = 1023;
const int JOY_MIN = 0;
const int GEAR_TOGGLE_BUTTON = 1;
const int FLAP_TOGGLE_BUTTON = 2;
const int TAKEOFF_THRESH = 500;




pitch_controller pitchController;
control_params pitchParams{
  -10,  // setpoint
  10,   // kp
  0, // ki
  0,    // kd
  10,   // sampleRate
  1023, // outputMax
  0     // ms pulswidth
};

roll_controller rollController;
control_params rollParams{
  0,    // setpoint
  20,   // kp
  0,    // ki
  10,   // kd
  10,   // sampleRate
  1023, // outputMax
  0     // ms pulswidth
};

heading_controller headingController;
control_params headingParams{
  40,   // setpoint
  2,    // kp
  0,    // ki
  0,    // kd
  10,   // sampleRate
  35,   // outputMax
  -35   // outputMin
};

altitude_controller altitudeController;
control_params altitudeParams{
  1000, // setpoint
  0.1,  // kp
  0, // ki
  0,    // kd
  10,   // sampleRate
  35,   // outputMax
  -25   // outputMin
};

ias_controller iasController;
control_params iasParams{
  350,  // setpoint
  10,   // kp
  0,    // ki
  0,    // kd
  10,   // sampleRate
  1023, // outputMax
  0     // outputMin
};

pilsim_state_params plane;

SerialTransfer feedback;
SerialTransfer datalog;





uint16_t throttle_command = JOY_MAX * 0.85;
float pitch_command = 512;
float roll_command  = 512;
float yaw_command   = 512;

float lat;
float lon;
float hdg;
float alt;
float ias;

nav_state navState = DISENGAGED;




bool handleData()
{
  if(feedback.available())
  {
    feedback.rxObj(plane);
    
    DEBUG_PORT.print(" "); DEBUG_PORT.print(plane.roll);
    DEBUG_PORT.print(" "); DEBUG_PORT.print(plane.pitch);
    DEBUG_PORT.print(" "); DEBUG_PORT.print(plane.hdg);
    DEBUG_PORT.print(" "); DEBUG_PORT.print(plane.alt);
    //DEBUG_PORT.print(" "); DEBUG_PORT.print(plane.lat);
    //DEBUG_PORT.print(" "); DEBUG_PORT.print(plane.lon);
    //DEBUG_PORT.print(" "); DEBUG_PORT.print(plane.ias);
    //DEBUG_PORT.print(" "); DEBUG_PORT.print(plane.flaps);
    //DEBUG_PORT.print(" "); DEBUG_PORT.println(plane.gear);
    DEBUG_PORT.println();

    return true;
  }

  return false;
}




void readNextWp()
{
  lat = atof(wpFile[wpfRow][wpfCol]);
  wpfCol++;
  lon = atof(wpFile[wpfRow][wpfCol]);
  wpfCol++;
  hdg = atof(wpFile[wpfRow][wpfCol]);
  wpfCol++;
  alt = atof(wpFile[wpfRow][wpfCol]);
  wpfCol++;
  ias = atof(wpFile[wpfRow][wpfCol]);
  wpfCol = 0;
  wpfRow++;
}




void handleControllers()
{
  static bool readWp = true;
  
  switch (navState)
  {
    case TAKEOFF:
    {
      if (readWp)
      {
        readNextWp();
        readWp = false;
      }
      
      if (distance(plane.lat, plane.lon, lat, lon) <= 15)
      {
        navState = TURN_I;
        readWp = true;
      }
      
      break;
    }
    
    case TURN_I:
    {
      if (readWp)
      {
        readNextWp();
        readWp = false;
      }
      
      break;
    }
    
    case STRAIGHT:
    {
      
      break;
    }
    
    case TURN_F:
    {
      
      break;
    }
    
    case FINAL:
    {
      
      break;
    }
    
    case DISENGAGED:
    {
      
      break;
    }
  }
  
  float temp_pitch_command = pitchController.compute(plane);
  if(pitchController.status)
    pitch_command = temp_pitch_command;

  float temp_roll_command  = rollController.compute(plane);
  if(rollController.status)
    roll_command = temp_roll_command;

  float temp_heading_command  = -headingController.compute(plane);
  if(headingController.status)
    rollController.setpoint = temp_heading_command;

  float temp_altitude_command  = -altitudeController.compute(plane);
  if(altitudeController.status)
    pitchController.setpoint = constrain(temp_altitude_command, -25, 90);

  float temp_ias_command  = iasController.compute(plane);
  if(iasController.status)
    throttle_command = temp_ias_command;
}




void sendJoyCommands()
{
  Joystick.X(pitch_command);
  Joystick.Y(roll_command);
  Joystick.Z(yaw_command);
  Joystick.Zrotate(throttle_command);
}




void simTick()
{
  if (handleData())
  {
    handleControllers();
    sendJoyCommands();
  }
}




void setupControllers()
{
  pitchController.begin(pitchParams);
  rollController.begin(rollParams);
  headingController.begin(headingParams);
  altitudeController.begin(altitudeParams);
  iasController.begin(iasParams);
}




void setupSim()
{
  feedback.begin(FEEDBACK_PORT);
  datalog.begin(DEBUG_PORT);
  
  setupControllers();
}
