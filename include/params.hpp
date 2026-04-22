#pragma once

#include <string>

namespace Params
{
    // Global Settings
    static constexpr int serialPort = 115200;

    // Joint I2C Addresses
    static constexpr int magnetometerAddr1 = 0x18;
    static constexpr int magnetometerAddr3 = 0x19;
    static constexpr int magnetometerAddr2 = 0x1A;

    namespace Pins
    {
        // Servo Pins
        static constexpr int servoPin1 = 13;
        static constexpr int servoPin2 = 12;

        // DC Motor Pins
        static constexpr int kMotor1_In1 = 27;
        static constexpr int kMotor1_In2 = 14;
        static constexpr int kMotor2_In1 = 26;
        static constexpr int kMotor2_In2 = 25;
    } // End of Pins

    namespace Motors
    {
        // Which hardware motor number (1 or 2) is left/right
        static constexpr int leftMotor = 1;
        static constexpr int rightMotor = 2;

        // Forward direction per motor: 1 = positive speed is forward, -1 = negative speed is forward
        // Flip these if a motor is mounted in reverse
        static constexpr int leftForward = 1;
        static constexpr int rightForward = -1;

        // Default drive speed (0-255)
        static constexpr int driveSpeed = 180;
    } // End of Motors

    namespace Wifi
    {
        static constexpr const char *wifi_name = "Nicol 116";
        static constexpr const char *wifi_password = "thenerdlab";
        // const char *ssid = "Nicol 116";
        // const char *password = "thenerdlab";
        static constexpr int portNumber = 80;
    } // End of Wifi

    namespace MagSensorValues
    {
        static constexpr float minMagnitude = 600.0;

        // Shared physical angles — same for all sensors
        static const float PHYSICAL_ANGLES[7] = {90.0, 60.0, 30.0, 0.0, -30.0, -60.0, -90.0};

        // Per-leg raw readings (sorted ascending to pair with PHYSICAL_ANGLES)
        static const float RAW_READINGS_LEG_1[7] = {-12.3284, -12.3132, -8.6361, -4.3975, 0.6608, 4.4029, 6.3600};
        static const float RAW_READINGS_LEG_2[7] = {-6.7369, -6.5691, -4.7230, -1.4843, 3.7297, 7.2333, 8.7587};
        static const float RAW_READINGS_LEG_3[7] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    }

} // End of Params
