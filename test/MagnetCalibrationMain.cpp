#include <Arduino.h>
#include "Magnetometer.hpp"
#include "params.hpp"

Magnetometer mag(0x0C);

void setup()
{
    Serial.begin(115200);
    mag.initalize();

    // Load the High-Precision data
    mag.setFullCalibration(
        Params::MagSensorValues::CALIBRATED_RAW_READINGS,
        Params::MagSensorValues::CALIBRATED_PHYSICAL_ANGLES);
}

void loop()
{
    float jointAngle = mag.getLinearHeading();
    Serial.print("Joint Angle: ");
    Serial.println(jointAngle);
    delay(20);
}
