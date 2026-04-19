#include <Arduino.h>
#include <ESP32Servo.h>
#include "Magnetometer.hpp"
#include "params.hpp"

// States
// cBp = child behind parent  -> sensor 1 (0x18) detecting
// cAp = child ahead parent   -> sensor 2 (0x1A) detecting

enum State
{
  cBp,
  cAp,
  UNKNOWN
};

Servo parentServo;
Servo childServo;
Magnetometer mag1(Params::magnetometerAddr1); // 0x18
Magnetometer mag2(Params::magnetometerAddr2); // 0x1A

void setMotorSpeed(int motor, int speed)
{
  int in1 = (motor == 1) ? Params::Pins::kMotor1_In1 : Params::Pins::kMotor2_In1;
  int in2 = (motor == 1) ? Params::Pins::kMotor1_In2 : Params::Pins::kMotor2_In2;
  speed = constrain(speed, -255, 255);
  if (speed >= 0)
  {
    analogWrite(in1, speed);
    analogWrite(in2, 0);
  }
  else
  {
    analogWrite(in1, 0);
    analogWrite(in2, -speed);
  }
}

State getState()
{
  bool s1 = mag1.isMagnetPresent();
  bool s2 = mag2.isMagnetPresent();
  if (s1 && !s2)
    return cBp;
  if (s2 && !s1)
    return cAp;
  return UNKNOWN;
}

const char *stateName(State s)
{
  if (s == cBp)
    return "cBp (child behind parent, sensor 1 active)";
  if (s == cAp)
    return "cAp (child ahead parent, sensor 2 active)";
  return "UNKNOWN";
}

// Blocks until both sensors stop detecting, or times out after 5s
bool waitForTransition()
{
  unsigned long start = millis();
  while (mag1.isMagnetPresent() || mag2.isMagnetPresent())
  {
    if (millis() - start > 5000)
    {
      Serial.println("WARNING: Transition timeout — sensors still detecting.");
      return false;
    }
    delay(50);
  }
  return true;
}

void doFlip()
{
  State s = getState();

  if (s == cBp)
  {
    Serial.println("cBp -> flip forward: child servo to 0...");
    childServo.write(0);
    delay(1500);

    Serial.println("Waiting for mid-transition (both sensors clear)...");
    waitForTransition();

    Serial.println("Mid-point reached. Returning child servo to 90...");
    childServo.write(90);
    delay(2000);

    Serial.println("Activating parent servo to 0...");
    parentServo.write(0);
    delay(1500);
    parentServo.write(90);
    delay(1500);
  }
  else if (s == cAp)
  {
    Serial.println("cAp -> flip forward: parent servo to 180...");
    parentServo.write(180);
    delay(1500);

    Serial.println("Waiting for mid-transition (both sensors clear)...");
    waitForTransition();

    Serial.println("Mid-point reached. Returning parent servo to 90...");
    parentServo.write(90);
    delay(2000);

    Serial.println("Activating child servo to 180...");
    childServo.write(180);
    delay(1500);
    childServo.write(90);
    delay(1500);
  }
  else
  {
    Serial.println("Cannot flip — state is UNKNOWN. Check sensor readings.");
  }
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  pinMode(Params::Pins::kMotor1_In1, OUTPUT);
  pinMode(Params::Pins::kMotor1_In2, OUTPUT);
  pinMode(Params::Pins::kMotor2_In1, OUTPUT);
  pinMode(Params::Pins::kMotor2_In2, OUTPUT);
  setMotorSpeed(1, 0);
  setMotorSpeed(2, 0);

  parentServo.attach(Params::Pins::servoPin1);
  childServo.attach(Params::Pins::servoPin2);
  parentServo.write(90);
  childServo.write(90);

  mag1.initalize();
  mag2.initalize();
  mag1.setFullCalibration(
      Params::MagSensorValues::RAW_READINGS_LEG_1,
      Params::MagSensorValues::PHYSICAL_ANGLES);
  mag2.setFullCalibration(
      Params::MagSensorValues::RAW_READINGS_LEG_2,
      Params::MagSensorValues::PHYSICAL_ANGLES);

  Serial.println("Ready.");
  Serial.println("Commands: r=read | state=current state | flip=execute one flip");
  Serial.println("          p<angle>=parent servo | c<angle>=child servo");
}

void loop()
{
  if (Serial.available() > 0)
  {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.equalsIgnoreCase("r"))
    {
      Serial.print("Mag1 (0x18): ");
      Serial.print(mag1.getMagnitude(), 2);
      Serial.println(mag1.isMagnetPresent() ? " [PRESENT]" : " [NONE]");
      Serial.print("Mag2 (0x1A): ");
      Serial.print(mag2.getMagnitude(), 2);
      Serial.println(mag2.isMagnetPresent() ? " [PRESENT]" : " [NONE]");
    }
    else if (input.equalsIgnoreCase("state"))
    {
      Serial.println(stateName(getState()));
    }
    else if (input.equalsIgnoreCase("flip"))
    {
      doFlip();
    }
    else if (input.length() > 1 && input.charAt(0) == 'p')
    {
      int angle = constrain(input.substring(1).toInt(), 0, 180);
      parentServo.write(angle);
      Serial.print("Parent servo -> ");
      Serial.println(angle);
    }
    else if (input.length() > 1 && input.charAt(0) == 'c')
    {
      int angle = constrain(input.substring(1).toInt(), 0, 180);
      childServo.write(angle);
      Serial.print("Child servo -> ");
      Serial.println(angle);
    }
    else
    {
      Serial.println("Unknown command. Use: r | state | flip | p<angle> | c<angle>");
    }
  }
}
