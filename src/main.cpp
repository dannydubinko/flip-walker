#include <Arduino.h>

#include "Magnetometer.hpp"
#include <ESP32Servo.h>

#include "password.hpp"
#include "params.hpp"

Magnetometer magnetometer(Params::magnetometerAddr1);
Servo myservo1;
Servo myservo2;

void setup(void)
{
  Serial.begin(115200);
  /* Wait for serial on USB platforms. */
  while (!Serial)
  {
    delay(10);
  }

  myservo1.attach(Params::Pins::servoPin1);
  myservo2.attach(Params::Pins::servoPin2);

  magnetometer.initalize(
      MLX90393_GAIN_1X, MLX90393_RES_16,
      MLX90393_OSR_0, MLX90393_FILTER_0);
}

void loop(void)
{
  float heading{magnetometer.getHeading()};
  Serial.println(heading);

  myservo1.write(90);
  myservo2.write(90);
}
