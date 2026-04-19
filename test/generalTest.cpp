#include <Arduino.h>
#include <ESP32Servo.h>
#include "Magnetometer.hpp"
#include "params.hpp"

#define MOTOR1_IN1 27
#define MOTOR1_IN2 14
#define MOTOR2_IN1 26
#define MOTOR2_IN2 25

Servo parentServo;
Servo childServo;
Magnetometer mag1(Params::magnetometerAddr1);
Magnetometer mag2(Params::magnetometerAddr2);

void setMotorSpeed(int motor, int speed)
{
    int in1 = (motor == 1) ? MOTOR1_IN1 : MOTOR2_IN1;
    int in2 = (motor == 1) ? MOTOR1_IN2 : MOTOR2_IN2;
    speed = constrain(speed, -255, 255);
    if (speed >= 0) { analogWrite(in1, speed); analogWrite(in2, 0); }
    else            { analogWrite(in1, 0);     analogWrite(in2, -speed); }
}

void setup()
{
    Serial.begin(115200);
    while (!Serial) delay(10);

    pinMode(MOTOR1_IN1, OUTPUT); pinMode(MOTOR1_IN2, OUTPUT);
    pinMode(MOTOR2_IN1, OUTPUT); pinMode(MOTOR2_IN2, OUTPUT);

    parentServo.attach(Params::Pins::servoPin1);
    childServo.attach(Params::Pins::servoPin2);
    parentServo.write(90);
    childServo.write(90);

    mag1.initalize();
    mag2.initalize();

    Serial.println("=== General Test ===");
    Serial.println("Commands:");
    Serial.println("  r        - read raw heading from all sensors");
    Serial.println("  p<0-180> - set parent servo angle");
    Serial.println("  c<0-180> - set child servo angle");
    Serial.println("  m1<-255:255> - set motor 1 speed");
    Serial.println("  m2<-255:255> - set motor 2 speed");
    Serial.println("  stop     - stop both motors");
}

void loop()
{
    if (Serial.available() > 0)
    {
        String input = Serial.readStringUntil('\n');
        input.trim();

        if (input.equalsIgnoreCase("r"))
        {
            float r1 = mag1.getRawHeading();
            float r2 = mag2.getRawHeading();
            Serial.print("Sensor 1 (0x"); Serial.print(Params::magnetometerAddr1, HEX);
            Serial.print("): "); Serial.print(r1, 4); Serial.println(" deg");
            Serial.print("Sensor 2 (0x"); Serial.print(Params::magnetometerAddr2, HEX);
            Serial.print("): "); Serial.print(r2, 4); Serial.println(" deg");
        }
        else if (input.length() > 1 && input.charAt(0) == 'p')
        {
            int angle = constrain(input.substring(1).toInt(), 0, 180);
            parentServo.write(angle);
            Serial.print("Parent servo -> "); Serial.println(angle);
        }
        else if (input.length() > 1 && input.charAt(0) == 'c')
        {
            int angle = constrain(input.substring(1).toInt(), 0, 180);
            childServo.write(angle);
            Serial.print("Child servo -> "); Serial.println(angle);
        }
        else if (input.length() > 2 && input.startsWith("m1"))
        {
            int speed = input.substring(2).toInt();
            setMotorSpeed(1, speed);
            Serial.print("Motor 1 -> "); Serial.println(speed);
        }
        else if (input.length() > 2 && input.startsWith("m2"))
        {
            int speed = input.substring(2).toInt();
            setMotorSpeed(2, speed);
            Serial.print("Motor 2 -> "); Serial.println(speed);
        }
        else if (input.equalsIgnoreCase("stop"))
        {
            setMotorSpeed(1, 0);
            setMotorSpeed(2, 0);
            Serial.println("Motors stopped.");
        }
        else
        {
            Serial.println("Unknown command. Type r, p<angle>, c<angle>, m1<speed>, m2<speed>, or stop.");
        }
    }
}
