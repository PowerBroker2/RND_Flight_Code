#pragma once




struct __attribute__((__packed__)) telem_struct
{
	uint16_t altitude;      //cm
	float courseAngleIMU;   //degrees
	float rollAngle;        //degrees (positive --> roll right)
	float pitchAngle;       //degrees (positive --> nose up)
	uint16_t pitotPressure; //unitless             MSB                                                                        LSB
	uint8_t validFlags;     //bit encoded bools (reserved, reserved, imuRecent, imuValid, fixRecent, fixValid, linkValid, manualControl)
	float latitude;         //dd
	float longitude;        //dd
	uint16_t UTC_year;      //y
	uint8_t UTC_month;      //M
	uint8_t UTC_day;        //d
	uint8_t UTC_hour;       //h
	uint8_t UTC_minute;     //m
	float UTC_second;       //s
	float speedOverGround;  //knots
	float courseOverGround; //degrees
} telem;




struct __attribute__((__packed__)) inputs_struct
{
	bool limiter_enable;        //enables and disables pitch and bank limiter
	bool manual_control_enable; //enables and disables manual flight control
	uint16_t pitch;
	uint16_t roll;
	uint16_t yaw;
	uint16_t throttle;
	uint16_t autopilot;
	uint16_t limiter;
	uint16_t gear;
	uint16_t flaps;
	uint16_t ec1;
  uint16_t ec2;
  uint16_t ec3;
  uint16_t ec4;
} inputs;




struct __attribute__((__packed__)) sec_interface_struct
{
  bool error;
  uint8_t type;
  uint8_t pin;
  uint16_t command;
  uint16_t response;
} secInterface;
