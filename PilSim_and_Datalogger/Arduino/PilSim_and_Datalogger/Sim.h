#pragma once
#include "SerialTransfer.h"
#include "Autopilot.h"
#include "Log.h"




const int JOY_MAX = 1023;
const int JOY_MIN = 0;
const int GEAR_TOGGLE_BUTTON = 1;
const int FLAP_TOGGLE_BUTTON = 2;
const int GROUND_MSL = 59;




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
    
    /*DEBUG_PORT.print("Roll: "); DEBUG_PORT.println(plane.roll);
    DEBUG_PORT.print("Pitch: "); DEBUG_PORT.println(plane.pitch);
    DEBUG_PORT.print("Heading: "); DEBUG_PORT.println(plane.hdg);
    DEBUG_PORT.print("Altitude: "); DEBUG_PORT.println(plane.alt);
    DEBUG_PORT.print("Latitude: "); DEBUG_PORT.println(plane.lat);
    DEBUG_PORT.print("Longitude: "); DEBUG_PORT.println(plane.lon);
    DEBUG_PORT.print("IAS: "); DEBUG_PORT.println(plane.ias);
    DEBUG_PORT.print("Flaps: "); DEBUG_PORT.println(plane.flaps);
    DEBUG_PORT.print("Gear: "); DEBUG_PORT.println(plane.gear);
    DEBUG_PORT.println();*/

    return true;
  }

  return false;
}




void readNextWp()
{
  wpfCol = 0;
  
  while (wpfCol < numCols)
  {
    if (!strcmp(wpFile[0][wpfCol], "lat"))
      lat = atof(wpFile[wpfRow][wpfCol]);
    else if (!strcmp(wpFile[0][wpfCol], "lon"))
      lon = atof(wpFile[wpfRow][wpfCol]);
    else if (!strcmp(wpFile[0][wpfCol], "hdg"))
      hdg = atof(wpFile[wpfRow][wpfCol]);
    else if (!strcmp(wpFile[0][wpfCol], "alt"))
      alt = atof(wpFile[wpfRow][wpfCol]);
    else if (!strcmp(wpFile[0][wpfCol], "ias"))
      ias = atof(wpFile[wpfRow][wpfCol]);
    
    wpfCol++;
  }
  
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

      pitchController.setpoint   = 20;
      headingController.setpoint = heading(plane.lat, plane.lon, lat, lon);
      iasController.setpoint     = ias;

      float temp_pitch_command = map(pitchController.compute(plane), JOY_MIN, JOY_MAX, JOY_MAX, JOY_MIN);
      if(pitchController.status)
        pitch_command = temp_pitch_command;
    
      float temp_heading_command  = headingController.compute(plane);
      if(headingController.status)
        rollController.setpoint = temp_heading_command;

      float temp_roll_command  = map(rollController.compute(plane), JOY_MIN, JOY_MAX, JOY_MAX, JOY_MIN);
      if(rollController.status)
        roll_command = temp_roll_command;
    
      float temp_ias_command  = iasController.compute(plane);
      if(iasController.status)
        throttle_command = constrain(temp_ias_command, JOY_MIN, JOY_MAX);
        
      static bool raiseGear = true;

      if ((plane.alt > (GROUND_MSL + 20)) && raiseGear)
      {
        Joystick.button(1, 1);
        delay(100);
        Joystick.button(1, 0);
        
        raiseGear = false;
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

      pitchController.setpoint    = degrees(atan((alt - plane.alt) / distance(plane.lat, plane.lon, lat, lon, false)));
      headingController.setpoint  = heading(plane.lat, plane.lon, lat, lon);
      iasController.setpoint      = ias;

      float temp_pitch_command = map(pitchController.compute(plane), JOY_MIN, JOY_MAX, JOY_MAX, JOY_MIN);
      if(pitchController.status)
        pitch_command = temp_pitch_command;
    
      float temp_heading_command  = headingController.compute(plane);
      if(headingController.status)
        rollController.setpoint = temp_heading_command;

      float temp_roll_command  = map(rollController.compute(plane), JOY_MIN, JOY_MAX, JOY_MAX, JOY_MIN);
      if(rollController.status)
        roll_command = temp_roll_command;
    
      float temp_ias_command  = iasController.compute(plane);
      if(iasController.status)
        throttle_command = constrain(temp_ias_command, JOY_MIN, JOY_MAX);
      
      if (distance(plane.lat, plane.lon, lat, lon) <= 15)
      {
        //navState = STRAIGHT;
        readWp = true;
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
    
    default:
    {
      navState = TAKEOFF;
      break;
    }
  }
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
