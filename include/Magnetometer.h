#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H

#include "Adafruit_MLX90393.h"

class Magnetometer {
    private:
    Adafruit_MLX90393 m_sensor;
    float m_sensorAddr{};

    public:
    Magnetometer(int sensorAddr);

    void initalize(
        enum mlx90393_gain gain, 
        enum mlx90393_resolution resolution, 
        enum mlx90393_oversampling sampling, 
        enum mlx90393_filter filter);

    void calibrate();
    float getMagnitude();
    float getHeading();
    float getDistance(float K);

};

#endif
