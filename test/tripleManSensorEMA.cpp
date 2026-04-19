#include <Arduino.h>
#include "Magnetometer.hpp"
#include "password.hpp"
#include "params.hpp"
#include "HeadingEMA.hpp" // Updated to .hpp

Magnetometer mag1(Params::magnetometerAddr1);
Magnetometer mag2(Params::magnetometerAddr2);
Magnetometer mag3(Params::magnetometerAddr3);

HeadingEMA filter1(0.1f);
HeadingEMA filter2(0.1f);
HeadingEMA filter3(0.1f);

void setup(void)
{
    Serial.begin(115200);
    while (!Serial)
    {
        delay(10);
    }

    mag1.initalize(MLX90393_GAIN_2X, MLX90393_RES_16, MLX90393_OSR_0, MLX90393_FILTER_2);
    mag2.initalize(MLX90393_GAIN_2X, MLX90393_RES_16, MLX90393_OSR_0, MLX90393_FILTER_2);
    mag3.initalize(MLX90393_GAIN_2X, MLX90393_RES_16, MLX90393_OSR_0, MLX90393_FILTER_2);

    mag1.setCalibration(0.0, 0.0, 0.0);
    mag2.setCalibration(0.0, 0.0, 0.0);
    mag3.setCalibration(0.0, 0.0, 0.0);
}

void loop(void)
{
    float smoothed1 = filter1(mag1.getRawHeading());
    float smoothed2 = filter2(mag2.getRawHeading());
    float smoothed3 = filter3(mag3.getRawHeading());

    Serial.print("Sensor1:");
    Serial.print(smoothed1);
    Serial.print("\tSensor2:");
    Serial.print(smoothed2);
    Serial.print("\tSensor3:");
    Serial.println(smoothed3);

    delay(15);
}
