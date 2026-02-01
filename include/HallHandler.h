#ifndef HALL_HANDLER_H
#define HALL_HANDLER_H

class HallHandler {
private:
    int _analogPin;
    float _vcc;
    float _zeroFieldVoltage;

public:
    // Constructor
    HallHandler(int pin, float vcc = 3.3);

    // Initialize sensor
    void begin();

    // Calibration: Sets the current reading as the "zero" point
    void calibrate();

    // Returns raw voltage
    float getVoltage();

    // Returns field strength in Gauss
    float getFieldStrength(float sensitivity_mv_g = 1.4);
};

#endif
