#include <Arduino.h>
#include "Magnetometer.hpp"
#include "params.hpp"

Magnetometer mag1(Params::magnetometerAddr1);
Magnetometer mag2(Params::magnetometerAddr2);

void setup()
{
    Serial.begin(115200);
    while (!Serial) delay(10);

    pinMode(Params::Pins::kMotor1_In1, OUTPUT);
    pinMode(Params::Pins::kMotor1_In2, OUTPUT);
    pinMode(Params::Pins::kMotor2_In1, OUTPUT);
    pinMode(Params::Pins::kMotor2_In2, OUTPUT);
    analogWrite(Params::Pins::kMotor1_In1, 0);
    analogWrite(Params::Pins::kMotor1_In2, 0);
    analogWrite(Params::Pins::kMotor2_In1, 0);
    analogWrite(Params::Pins::kMotor2_In2, 0);

    mag1.initalize();
    mag2.initalize();
}

void loop()
{
    Serial.print("Mag1: "); Serial.print(mag1.getMagnitude(), 2);
    Serial.print(mag1.isMagnetPresent() ? " [PRESENT]" : " [NONE]   ");
    Serial.print("  |  Mag2: "); Serial.print(mag2.getMagnitude(), 2);
    Serial.println(mag2.isMagnetPresent() ? " [PRESENT]" : " [NONE]   ");
    delay(100);
}
