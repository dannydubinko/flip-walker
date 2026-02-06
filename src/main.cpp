#include <Arduino.h>
#include "Magnetometer.h"

  Magnetometer magnetometer(0x18);

void setup(void)
{
  Serial.begin(115200);
  /* Wait for serial on USB platforms. */
  while (!Serial) {
      delay(10);
  }

  magnetometer.initalize(
    MLX90393_GAIN_1X, MLX90393_RES_16, 
    MLX90393_OSR_0, MLX90393_FILTER_0);
}

void loop(void) {
  float heading{magnetometer.getHeading()};
  Serial.println(heading);
}
