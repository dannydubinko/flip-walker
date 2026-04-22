#include "Magnetometer.hpp"
#include <Arduino.h>

Magnetometer::Magnetometer(int sensorAddr)
    : m_sensorAddr{sensorAddr}
{
  m_sensor = Adafruit_MLX90393();
}

void Magnetometer::initalize(
    enum mlx90393_gain gain,
    enum mlx90393_resolution resolution,
    enum mlx90393_oversampling sampling,
    enum mlx90393_filter filter)
{
  Serial.println("Starting Adafruit MLX90393 Demo");

  if (!m_sensor.begin_I2C(m_sensorAddr))
  { // hardware I2C mode, can pass in address & alt Wire
    // if (! sensor.begin_SPI(MLX90393_CS)) {  // hardware SPI mode
    Serial.println("No sensor found ... check your wiring?");
    while (1)
    {
      delay(10);
    }
  }
  Serial.println("Found a MLX90393 sensor");

  m_sensor.setGain(gain);
  // You can check the gain too
  Serial.print("Gain set to: ");
  switch (m_sensor.getGain())
  {
  case MLX90393_GAIN_1X:
    Serial.println("1 x");
    break;
  case MLX90393_GAIN_1_33X:
    Serial.println("1.33 x");
    break;
  case MLX90393_GAIN_1_67X:
    Serial.println("1.67 x");
    break;
  case MLX90393_GAIN_2X:
    Serial.println("2 x");
    break;
  case MLX90393_GAIN_2_5X:
    Serial.println("2.5 x");
    break;
  case MLX90393_GAIN_3X:
    Serial.println("3 x");
    break;
  case MLX90393_GAIN_4X:
    Serial.println("4 x");
    break;
  case MLX90393_GAIN_5X:
    Serial.println("5 x");
    break;
  }

  // 1. Lower the Resolution slightly (Speed up processing)
  // (Change RES_17/16 to RES_16 for all)
  m_sensor.setResolution(MLX90393_X, resolution);
  m_sensor.setResolution(MLX90393_Y, resolution);
  m_sensor.setResolution(MLX90393_Z, resolution);

  // 2. Reduce Oversampling (Don't average so much)
  // Change OSR_3 to OSR_0 or OSR_1
  m_sensor.setOversampling(sampling);

  // 3. Lower the Filter (The most important fix)
  // Change FILTER_5 to FILTER_1 or FILTER_0
  m_sensor.setFilter(filter);
}

void Magnetometer::calibrate() {}

float Magnetometer::getMagnitude()
{
  float x{};
  float y{};
  float z{};
  m_sensor.readData(&x, &y, &z);
  return sqrt(sq(x) + sq(y) + sq(z));
  ;
}

float Magnetometer::getHeading()
{
  float x{};
  float y{};
  float z{};

  m_sensor.readData(&x, &y, &z);

  float heading = atan2(x, z);

  float headingDegrees = (heading) * 180.0 / PI;

  // 3. Fix negative angles (so -90 becomes 270)
  //   if (headingDegrees < 0) {
  //     headingDegrees += 360;
  //   }
  return headingDegrees;
}

float Magnetometer::getDistance(float K)
{
  return cbrt(K / getMagnitude());
}
