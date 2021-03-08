#pragma once
#include "Arduino.h"
#include "Autopilot.h"
#include "Controls.h"




uint16_t throttle_command = MS_LOW;
float pitch_command = MS_MID;
float roll_command  = MS_MID;
float yaw_command   = MS_MID;




pitch_controller pitchController;
control_params pitchParams{
  -10,  // setpoint
  10,   // kp
  0,    // ki
  0,    // kd
  10,   // sampleRate
  MS_HIGH, // outputMax
  MS_LOW   // ms pulswidth
};

roll_controller rollController;
control_params rollParams{
  0,    // setpoint
  20,   // kp
  0,    // ki
  10,   // kd
  10,   // sampleRate
  MS_HIGH, // outputMax
  MS_LOW   // outputMin
};

heading_controller headingController;
control_params headingParams{
  0,    // setpoint
  2,    // kp
  0,    // ki
  0,    // kd
  10,   // sampleRate
  ANGL_HIGH, // outputMax
  ANGL_LOW   // outputMin
};

altitude_controller altitudeController;
control_params altitudeParams{
  100,  // setpoint
  0.1,  // kp
  0,    // ki
  0,    // kd
  10,   // sampleRate
  ANGL_HIGH, // outputMax
  ANGL_LOW   // outputMin
};

ias_controller iasController;
control_params iasParams{
  40,   // setpoint
  10,   // kp
  0,    // ki
  0,    // kd
  10,   // sampleRate
  MS_HIGH, // outputMax
  MS_LOW   // outputMin
};

state_params plane;

struct inputs{
  float pitch = 0;
  float roll  = 0;
  float hdg   = 0;
  float alt   = 0;
  float ias   = 0;
} setpoints;




bool engageAP = false;




void setupControllers()
{
  pitchController.begin(pitchParams);
  rollController.begin(rollParams);
  headingController.begin(headingParams);
  altitudeController.begin(altitudeParams);
  iasController.begin(iasParams);
}




void handleControllers()
{
  if(engageAP && (validFlags & IMU_RECENT) && (validFlags & IMU_VALID) && (validFlags & FIX_RECENT) && (validFlags & FIX_VALID))
  {
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
      pitchController.setpoint = temp_altitude_command;
  
    float temp_ias_command  = iasController.compute(plane);
    if(iasController.status)
      throttle_command = temp_ias_command;
  
    setpoints.pitch = -pitchController.setpoint;
    setpoints.roll  = -rollController.setpoint;
    setpoints.hdg   = headingController.setpoint;
    setpoints.alt   = altitudeController.setpoint;
    setpoints.ias   = iasController.setpoint;
  }
  else
  {
    pitchServo.writeMicroseconds(pitchPulseLen);
    rollServo.writeMicroseconds(rollPulseLen);
    yawServo.writeMicroseconds(yawPulseLen);
    throttleServo.writeMicroseconds(throttlePulseLen);
    camServo.writeMicroseconds(camPulseLen);
  }
}
