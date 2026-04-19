#include "Magnetometer.hpp"
#include <Arduino.h>

Magnetometer::Magnetometer(int sensorAddr)
    : m_sensorAddr{sensorAddr}, m_isCalibrated{false}
{
}

void Magnetometer::initalize(
    enum mlx90393_gain gain,
    enum mlx90393_resolution resolution,
    enum mlx90393_oversampling sampling,
    enum mlx90393_filter filter)
{
  if (!m_sensor.begin_I2C(m_sensorAddr))
  {
    Serial.print("No MLX90393 found at address: 0x");
    Serial.println(m_sensorAddr, HEX);
    while (1)
      delay(10);
  }

  m_sensor.setGain(gain);
  m_sensor.setResolution(MLX90393_X, resolution);
  m_sensor.setResolution(MLX90393_Y, resolution);
  m_sensor.setResolution(MLX90393_Z, resolution);
  m_sensor.setOversampling(sampling);
  m_sensor.setFilter(filter);
}

// ---------------------------------------------------------
// CALIBRATION SETTERS
// ---------------------------------------------------------

void Magnetometer::setCalibration(float rawNeg90, float raw0, float rawPos90)
{
  m_rawAtNeg90 = rawNeg90;
  m_rawAt0 = raw0;
  m_rawAtPos90 = rawPos90;
  m_isCalibrated = true;
}

void Magnetometer::setFullCalibration(const float *rawValues, const float *angles)
{
  for (int i = 0; i < NUM_POINTS; i++)
  {
    m_rawReadings[i] = rawValues[i];
    m_physicalAngles[i] = angles[i];
  }

  // Set the 0-degree reference (index 3 for a 7-point array)
  m_rawAt0 = m_rawReadings[NUM_POINTS / 2];
  m_isCalibrated = true;
}

// ---------------------------------------------------------
// HEADING LOGIC
// ---------------------------------------------------------

float Magnetometer::getRawHeading()
{
  float x, y, z;
  m_sensor.readData(&x, &y, &z);

  // Calculate raw heading from X and Z axes
  // Note: Using atan2 handles the quadrant logic automatically
  return atan2(x, z) * 180.0 / PI;
}

/**
 * Basic 3-point Piecewise Mapping
 * Best for quick tests or rough positioning.
 */
float Magnetometer::getHeading()
{
  float rawHeading = getRawHeading();

  if (!m_isCalibrated)
    return rawHeading;

  float calibratedHeading;
  if (rawHeading <= m_rawAt0)
  {
    // Map from [Neg90, 0] raw to [-90, 0] physical
    calibratedHeading = map(rawHeading * 100, m_rawAtNeg90 * 100, m_rawAt0 * 100, -9000, 0) / 100.0;
  }
  else
  {
    // Map from [0, Pos90] raw to [0, 90] physical
    calibratedHeading = map(rawHeading * 100, m_rawAt0 * 100, m_rawAtPos90 * 100, 0, 9000) / 100.0;
  }

  return calibratedHeading;
}

/**
 * High-Precision Linear Interpolation (7-point LUT)
 * Use this for the FlipWalker gait to account for magnetic warping.
 */
float Magnetometer::getLinearHeading()
{
  float currentRaw = getRawHeading();

  if (!m_isCalibrated)
    return currentRaw;

  // 1. Normalize relative to 0-point to handle Atan2 +/-180 wrap-around
  float normRaw = currentRaw - m_rawAt0;
  while (normRaw > 180)
    normRaw -= 360;
  while (normRaw < -180)
    normRaw += 360;

  // Shift back to absolute space for array comparison
  currentRaw = normRaw + m_rawAt0;

  // 2. Linear Interpolation Lookup
  for (int i = 0; i < NUM_POINTS - 1; i++)
  {
    if (currentRaw >= m_rawReadings[i] && currentRaw <= m_rawReadings[i + 1])
    {
      float x0 = m_rawReadings[i];
      float x1 = m_rawReadings[i + 1];
      float y0 = m_physicalAngles[i];
      float y1 = m_physicalAngles[i + 1];

      // Formula: y = y0 + (x - x0) * (y1 - y0) / (x1 - x0)
      return y0 + (currentRaw - x0) * (y1 - y0) / (x1 - x0);
    }
  }

  // 3. Out-of-bounds Clamping
  if (currentRaw < m_rawReadings[0])
    return m_physicalAngles[0];
  if (currentRaw > m_rawReadings[NUM_POINTS - 1])
    return m_physicalAngles[NUM_POINTS - 1];

  return 0.0;
}

// ---------------------------------------------------------
// UTILITIES
// ---------------------------------------------------------

void Magnetometer::getRawAxes(float &x, float &y, float &z)
{
  m_sensor.readData(&x, &y, &z);
}

float Magnetometer::getMagnitude()
{
  float x, y, z;
  m_sensor.readData(&x, &y, &z);
  return sqrt(sq(x) + sq(y) + sq(z));
}

float Magnetometer::getDistance(float K)
{
  float mag = getMagnitude();
  if (mag <= 0)
    return 0.0;
  return cbrt(K / mag);
}
