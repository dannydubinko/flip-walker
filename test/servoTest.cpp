#include <Arduino.h>
#include <ESP32Servo.h>

// DC Motor Pins
#define MOTOR1_IN1 27
#define MOTOR1_IN2 14
#define MOTOR2_IN1 26
#define MOTOR2_IN2 25

// Servo Pins
static const int servoPin1 = 13;
static const int servoPin2 = 12;

// Create servo objects
Servo myservo1;
Servo myservo2;

void setMotorSpeed(int motor, int speed)
{
    int in1 = (motor == 1) ? MOTOR1_IN1 : MOTOR2_IN1;
    int in2 = (motor == 1) ? MOTOR1_IN2 : MOTOR2_IN2;

    // Standard DC motor logic: one pin PWM, the other 0
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

    // Initialize DC Motor Pins
    pinMode(MOTOR1_IN1, OUTPUT);
    pinMode(MOTOR1_IN2, OUTPUT);
    pinMode(MOTOR2_IN1, OUTPUT);
    pinMode(MOTOR2_IN2, OUTPUT);

    // Initialize Servos
    myservo1.attach(servoPin1);
    myservo2.attach(servoPin2);

    Serial.println("System Ready. Enter a servo angle (0-180) in the Serial Monitor:");
}

void loop()
{
    // 1. DC Motor Logic
    setMotorSpeed(1, 150); // Motor 1 forward at speed 50 (battery side motor)
    setMotorSpeed(2, 0);   // Motor 2 stopped

    // 2. Serial Input for Servos
    if (Serial.available() > 0)
    {
        // Read the incoming integer
        int angle = Serial.parseInt();

        // Check if the angle is valid (0 to 180)
        if (angle >= 0 && angle <= 180)
        {
            Serial.print("Moving servos to: ");
            Serial.println(angle);

            myservo1.write(angle); // main body servo
            // myservo2.write(angle); //sub body servo
        }
        else
        {
            // Flush remaining data if input was invalid or just a newline
            while (Serial.available() > 0)
                Serial.read();
        }
    }

    delay(20);
}
