#include <Arduino.h>
#include "HallHandler.h"

HallHandler::HallHandler(int pin, float vcc) {
    _analogPin = pin;
    _vcc = vcc; // This should be 3.3 because the ESP32 ADC range is 3.3V
    _zeroFieldVoltage = 1.65; // Default middle
}

void HallHandler::begin() {
    pinMode(_analogPin, INPUT);
    analogReadResolution(12); 
}

void HallHandler::calibrate() {
    Serial.println("Calibrating Hall Sensor... Keep magnets away.");
    float sum = 0;
    int samples = 500; // More samples for a steady zero
    for(int i = 0; i < samples; i++) {
        // Convert raw ADC (0-4095) to Voltage (0-3.3)
        sum += (analogRead(_analogPin) / 4095.0) * 3.3; 
        delay(2);
    }
    _zeroFieldVoltage = sum / (float)samples; // Fixed division
    Serial.print("Zero Point Set to: "); Serial.println(_zeroFieldVoltage);
}

float HallHandler::getVoltage() {
    float sum = 0;
    int samples = 100; 
    for(int i = 0; i < samples; i++) {
        sum += analogRead(_analogPin);
        delayMicroseconds(50); // Give the ADC time to breathe
    }
    float avgRaw = sum / (float)samples;
    return (avgRaw / 4095.0) * 3.3; 
}

float HallHandler::getFieldStrength(float sensitivity_mv_g) {
    float voltage = getVoltage();
    
    // Use the calibrated _zeroFieldVoltage instead of hardcoded 2.5
    float strength = (voltage - _zeroFieldVoltage) / (sensitivity_mv_g / 1000.0);
    
    return strength;
}
