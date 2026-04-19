#pragma once

#include "Adafruit_MLX90393.h"

// 13 points covers -90 to 90 in 15-degree increments
#define NUM_POINTS 7

class Magnetometer
{
private:
    Adafruit_MLX90393 m_sensor;
    int m_sensorAddr;

    // Standard 3-point calibration data
    float m_rawAtNeg90 = -90.0;
    float m_rawAt0 = 0.0;
    float m_rawAtPos90 = 90.0;
    bool m_isCalibrated = false;

    // Full LUT Calibration arrays for getLinearHeading()
    float m_rawReadings[NUM_POINTS];
    float m_physicalAngles[NUM_POINTS];

public:
    Magnetometer(int sensorAddr);

    void initalize(
        enum mlx90393_gain gain = MLX90393_GAIN_1X,
        enum mlx90393_resolution resolution = MLX90393_RES_16,
        enum mlx90393_oversampling sampling = MLX90393_OSR_3,
        enum mlx90393_filter filter = MLX90393_FILTER_5);

    void setCalibration(float rawNeg90, float raw0, float rawPos90);
    void setFullCalibration(const float *rawValues, const float *angles);

    void getRawAxes(float &x, float &y, float &z);
    float getMagnitude();
    float getRawHeading();
    float getDistance(float K);
    bool isMagnetPresent();

    float getHeading();       // 3-point mapping — returns NAN if no magnet
    float getLinearHeading(); // 7-point High-Precision LUT — returns NAN if no magnet
};
