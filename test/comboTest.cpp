#include <Arduino.h>
#include <ESP32Servo.h>
#include "Magnetometer.hpp"
#include "params.hpp"

// --- Pin Definitions ---
// DC Motor 1 (Right / Battery Side)
#define MOTOR1_IN1 27
#define MOTOR1_IN2 14
// DC Motor 2 (Left)
#define MOTOR2_IN1 26
#define MOTOR2_IN2 25

// Servo Pins
static const int servoPin1 = 13; // Main body servo
static const int servoPin2 = 12; // Sub body servo

// --- Object Initialization ---
Servo myservo1;
Servo myservo2;
Magnetometer mag1(Params::magnetometerAddr1);
Magnetometer mag2(Params::magnetometerAddr2);

/**
 * Sets the speed and direction for the DC motors.
 * @param motor: 1 for Right, 2 for Left
 * @param speed: -255 to 255 (Positive = Forward, Negative = Reverse)
 */
void setMotorSpeed(int motor, int speed)
{
    int in1 = (motor == 1) ? MOTOR1_IN1 : MOTOR2_IN1;
    int in2 = (motor == 1) ? MOTOR1_IN2 : MOTOR2_IN2;

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

void setup()
{
    Serial.begin(115200);

    // 1. Initialize DC Motor Pins
    pinMode(MOTOR1_IN1, OUTPUT);
    pinMode(MOTOR1_IN2, OUTPUT);
    pinMode(MOTOR2_IN1, OUTPUT);
    pinMode(MOTOR2_IN2, OUTPUT);

    // 2. Initialize Servos
    myservo1.attach(servoPin1);
    myservo2.attach(servoPin2);

    myservo1.write(90);
    myservo2.write(90);

    // 3. Initialize Magnetometer Sensor
    mag1.initalize();
    mag2.initalize();
    // Apply high-precision calibration data from params.hpp
    mag1.setFullCalibration(
        Params::MagSensorValues::RAW_READINGS_LEG_1,
        Params::MagSensorValues::PHYSICAL_ANGLES);
    mag2.setFullCalibration(
        Params::MagSensorValues::RAW_READINGS_LEG_1,
        Params::MagSensorValues::PHYSICAL_ANGLES);

    Serial.println("System Ready.");
    Serial.println("Enter a servo angle (0-180) to move the Main Body Servo.");
}

void loop()
{
    // --- 1. DC Motor Control ---
    setMotorSpeed(1, 0); // Set Right Motor (battery side) to forward speed 150
    setMotorSpeed(2, 0); // Keep Left Motor stopped

    // --- 2. Magnetometer Sensing ---
    // Read the current angle from the sensor
    float jointAngle = mag1.getRawHeading();
    float jointAngle2 = mag2.getRawHeading();

    Serial.print("Joint Angle: ");
    Serial.print(jointAngle);
    Serial.print(" | ");
    Serial.print("Joint Angle: ");
    Serial.print(jointAngle2);
    Serial.println("");

    // --- 3. Serial Input for Servo Control ---
    if (Serial.available() > 0)
    {
        // Read the incoming integer from Serial Monitor
        int angle = Serial.parseInt();

        // Validate angle range (0 to 180 degrees)
        if (angle >= 0 && angle <= 180)
        {
            Serial.print("-> Moving Main Body Servo to: ");
            Serial.println(angle);

            myservo1.write(angle); // Main body servo
            myservo2.write(angle); // Sub body servo
        }
        else
        {
            // Clear the buffer if input is invalid
            while (Serial.available() > 0)
                Serial.read();
        }
    }
    else
    {
        Serial.println("Waiting for Serial input...");
    }

    delay(20); // Short delay for stability
}
