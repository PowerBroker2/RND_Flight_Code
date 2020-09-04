#include "Servo.h"
#include "Adafruit_BNO055.h"
#include "Autopilot.h"

//#define DEBUG

const int MS_HIGH   = 2000;
const int MS_LOW    = 1000;
const int ANGL_HIGH = 30;
const int ANGL_LOW  = -30;

const int PITCH_SERVO_PIN      = 20;
const int PITCH_INPUT_PIN      = 2;
float pitchSetpoint            = 0;
volatile long pitchStartMicros = 0;
volatile long pitchPulseLen    = 0;
volatile bool pitchNewPulse    = 0;

const int ROLL_SERVO_PIN      = 16;
const int ROLL_INPUT_PIN      = 22;
float rollSetpoint            = 0;
volatile long rollStartMicros = 0;
volatile long rollPulseLen    = 0;
volatile bool rollNewPulse    = 0;

const int YAW_SERVO_PIN      = 21;
const int YAW_INPUT_PIN      = 3;
float yawSetpoint            = 0;
volatile long yawStartMicros = 0;
volatile long yawPulseLen    = 0;
volatile bool yawNewPulse    = 0;

const int THROTTLE_SERVO_PIN      = 17;
const int THROTTLE_INPUT_PIN      = 23;
volatile long throttleStartMicros = 0;
volatile long throttlePulseLen    = 0;
volatile bool throttleNewPulse    = 0;

const int AUTO  = 0;
const int LIMIT = 1;
const int MAN   = 2;
int flightSetting = MAN;

Adafruit_BNO055 bno = Adafruit_BNO055(55, BNO055_ADDRESS_A);

Servo pitchServo;
Servo rollServo;
Servo yawServo;
Servo throttleServo;

pitch_controller pitchController;
control_params pitchParams{
  10,   // Degrees
  50,   // Unitless
  10,   // Unitless
  0,    // Unitless
  50,   // Hz
  2000, // ms pulswidth
  1000, // ms pulswidth
};

roll_controller rollController;
control_params rollParams{
  0,    // Degrees
  50,   // Unitless
  10,   // Unitless
  0,    // Unitless
  50,   // Hz
  2000, // ms pulswidth
  1000, // ms pulswidth
};


void setup()
{
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  if (!bno.begin())
  {
    //there was a problem detecting the BNO055 ... check your connections
#ifdef DEBUG
    Serial.println(F("\tOoops, no BNO055 detected ... Check your wiring or I2C ADDR!"));
#endif

    //fatal error - halt code execution
    while (1);
  }
  else
  {
    delay(1000);
    bno.setExtCrystalUse(true);
 
#ifdef DEBUG
    Serial.println(F("\tBNO055 successfully initialized"));
#endif
  }

  pitchServo.attach(PITCH_SERVO_PIN);
  rollServo.attach(ROLL_SERVO_PIN);
  yawServo.attach(YAW_SERVO_PIN);
  throttleServo.attach(THROTTLE_SERVO_PIN);

  pitchController.begin(pitchParams);
  rollController.begin(rollParams);
  
  pinMode(PITCH_INPUT_PIN, INPUT);
  attachInterrupt(PITCH_INPUT_PIN, pitch_ISR, CHANGE);

  pinMode(ROLL_INPUT_PIN, INPUT);
  attachInterrupt(ROLL_INPUT_PIN, roll_ISR, CHANGE);

  pinMode(YAW_INPUT_PIN, INPUT);
  attachInterrupt(YAW_INPUT_PIN, yaw_ISR, CHANGE);

  pinMode(THROTTLE_INPUT_PIN, INPUT);
  attachInterrupt(THROTTLE_INPUT_PIN, throttle_ISR, CHANGE);
}


void loop()
{
  autopilot();

#ifdef DEBUG
  Serial.print("Course Angle: ");
  Serial.println(courseAngle);
  Serial.print("Pitch Angle: ");
  Serial.println(pitchAngle);
  Serial.print("Roll Angle: ");
  Serial.println(rollAngle);
  Serial.println();
#endif
  
  if(pitchNewPulse)
  {
    pitchNewPulse = false;
    pitchSetpoint = float_constrain(float_map(pitchPulseLen, MS_LOW, MS_HIGH, ANGL_LOW, ANGL_HIGH), ANGL_LOW, ANGL_HIGH);
    pitchController.setpoint = pitchSetpoint;

#ifdef DEBUG
    Serial.print("Pitch Pulse Length: ");
    Serial.println(pitchPulseLen);
    Serial.print("Pitch Setpoint: ");
    Serial.println(pitchSetpoint);
    Serial.println();
#endif
  }

  if(rollNewPulse)
  {
    rollNewPulse = false;
    rollSetpoint = float_constrain(float_map(rollPulseLen, MS_LOW, MS_HIGH, ANGL_LOW, ANGL_HIGH), ANGL_LOW, ANGL_HIGH);
    rollController.setpoint = rollSetpoint;

#ifdef DEBUG
    Serial.print("Roll Pulse Length: ");
    Serial.println(rollPulseLen);
    Serial.print("Roll Setpoint: ");
    Serial.println(rollSetpoint);
    Serial.println();
#endif
  }

  if(yawNewPulse)
  {
    yawNewPulse = false;
    yawSetpoint = float_constrain(float_map(yawPulseLen, MS_LOW, MS_HIGH, ANGL_LOW, ANGL_HIGH), ANGL_LOW, ANGL_HIGH);
    yawServo.write(yawPulseLen);

#ifdef DEBUG
    Serial.print("Yaw Pulse Length: ");
    Serial.println(yawPulseLen);
    Serial.print("Yaw Setpoint: ");
    Serial.println(yawSetpoint);
    Serial.println();
#endif
  }

  if(throttleNewPulse)
  {
    throttleNewPulse = false;
    throttleServo.write(throttlePulseLen);

#ifdef DEBUG
    Serial.print("Throttle Pulse Length: ");
    Serial.println(throttlePulseLen);
    Serial.println();
#endif
  }
}


void autopilot()
{
  auto vect = bno.getQuat().toEuler();
  float courseAngle =  vect.x() * (180 / M_PI);
  float pitchAngle  =  vect.y() * (180 / M_PI);
  float rollAngle   = -vect.z() * (180 / M_PI);

  if(!isnan(courseAngle) && !isnan(pitchAngle) && !isnan(rollAngle))
  {
    if(flightSetting == AUTO)
    {
      float newPitchCommand = pitchController.compute(pitchAngle, rollAngle, -1);
      
      if(pitchController.status)
        pitchServo.write(newPitchCommand);
  
      float newRollCommand = rollController.compute(pitchAngle, rollAngle, -1);
      
      if(rollController.status)
        rollServo.write(newRollCommand);
    }
    else if(flightSetting == LIMIT)
    {
      // TODO
    }
    else
    {
      pitchServo.write(pitchPulseLen);
      rollServo.write(rollPulseLen);
    }
  }
}


void pitch_ISR()
{
  if(digitalRead(PITCH_INPUT_PIN))
  {
    pitchStartMicros = micros();
    pitchNewPulse = false;
  }
  else
  {
    pitchPulseLen = micros() - pitchStartMicros;
    pitchNewPulse = true;
  }
}


void roll_ISR()
{
  if(digitalRead(ROLL_INPUT_PIN))
  {
    rollStartMicros = micros();
    rollNewPulse = false;
  }
  else
  {
    rollPulseLen = micros() - rollStartMicros;
    rollNewPulse = true;
  }
}


void yaw_ISR()
{
  if(digitalRead(YAW_INPUT_PIN))
  {
    yawStartMicros = micros();
    yawNewPulse = false;
  }
  else
  {
    yawPulseLen = micros() - yawStartMicros;
    yawNewPulse = true;
  }
}


void throttle_ISR()
{
  if(digitalRead(THROTTLE_INPUT_PIN))
  {
    throttleStartMicros = micros();
    throttleNewPulse = false;
  }
  else
  {
    throttlePulseLen = micros() - throttleStartMicros;
    throttleNewPulse = true;
  }
}
