#include <Arduino.h>
#include "Magnetometer.hpp"
#include "password.hpp"
#include "params.hpp"
#include "HeadingEMA.hpp" // Updated to .hpp

Magnetometer magnetometer(Params::magnetometerAddr3);

// Create the circular filter
HeadingEMA headingFilter(0.2f);

void setup(void)
{
    Serial.begin(115200);
    while (!Serial)
    {
        delay(10);
    }

    magnetometer.initalize(
        MLX90393_GAIN_2X, MLX90393_RES_16,
        MLX90393_OSR_0, MLX90393_FILTER_2);

    magnetometer.setCalibration(-90.25, -2.09, 93.28);
}

void loop(void)
{
    float rawHeading = magnetometer.getRawHeading();
    float filteredHeading = headingFilter(rawHeading);

    Serial.print("Raw:");
    Serial.print(rawHeading);
    Serial.print("\tFiltered:");
    Serial.println(filteredHeading);

    delay(15);
}
