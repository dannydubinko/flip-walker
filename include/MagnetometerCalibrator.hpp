#ifndef MAGNETOMETER_CALIBRATOR_HPP
#define MAGNETOMETER_CALIBRATOR_HPP

#include "Magnetometer.hpp"
#include <Arduino.h>

class MagnetometerCalibrator
{
public:
    MagnetometerCalibrator(Magnetometer &mag);

    void runInterativeCalibration(int numPoints = 7, const String &legLabel = "LEG_1");
    void printCalibrationArrays(const String &legLabel);

private:
    Magnetometer &m_mag;
    float *m_rawBuffer;
    float *m_angleBuffer;
    int m_numPoints;

    void waitForUser();
};

#endif
