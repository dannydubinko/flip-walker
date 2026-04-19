#include <Arduino.h>
#include "Magnetometer.hpp"
#include "params.hpp"

Magnetometer mag1(Params::magnetometerAddr1);
#define MOTOR1_IN1 27
#define MOTOR1_IN2 14
#define MOTOR2_IN1 26
#define MOTOR2_IN2 25

void setup()
{
    Serial.begin(115200);

    // DC Motor Control Pins and Output
    pinMode(MOTOR1_IN1, OUTPUT);
    pinMode(MOTOR1_IN2, OUTPUT);
    pinMode(MOTOR2_IN1, OUTPUT);
    pinMode(MOTOR2_IN2, OUTPUT);

    // Magnet Sensors
    mag1.initalize();

    // Load the High-Precision data
    mag1.setFullCalibration(
        Params::MagSensorValues::RAW_READINGS_LEG_1,
        Params::MagSensorValues::PHYSICAL_ANGLES);
}

void setMotorSpeed(int motor, int speed)
{
    int in1 = (motor == 1) ? MOTOR1_IN1 : MOTOR2_IN1;
    int in2 = (motor == 1) ? MOTOR1_IN2 : MOTOR2_IN2;

    analogWrite(in1, max(speed, 0));
    analogWrite(in2, max(-speed, 0));
}

void loop()
{
    setMotorSpeed(1, 50); // set motor speed right (battery side)
    setMotorSpeed(2, 0);  // set motor speed left
    // float jointAngle = mag1.getMagnitude();

    Serial.print("Joint Angle: ");
    // Serial.println(jointAngle);
    delay(20);
}
