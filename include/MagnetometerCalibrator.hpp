#ifndef MAGNETOMETER_CALIBRATOR_HPP
#define MAGNETOMETER_CALIBRATOR_HPP

#include "Magnetometer.hpp"
#include <Arduino.h>

class MagnetometerCalibrator
{
public:
    MagnetometerCalibrator(Magnetometer &mag);

    // Starts the interactive serial calibration
    void runInterativeCalibration(int numPoints = 13);

    // Utility to print the final arrays in a copy-paste format
    void printCalibrationArrays();

private:
    Magnetometer &m_mag;
    float *m_rawBuffer;
    float *m_angleBuffer;
    int m_numPoints;

    void waitForUser();
};

#endif
