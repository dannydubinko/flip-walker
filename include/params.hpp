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

    namespace Wifi
    {
        static constexpr const char *wifi_name = "BELL549";
        static constexpr const char *wifi_password = "1FFCC99A399E";
        // const char *ssid = "Nicol 116";
        // const char *password = "thenerdlab";
        static constexpr int portNumber = 80;
    } // End of Wifi

    namespace MagSensorValues
    {
        // Reversed so the mapping aligns with the ascending raw readings
        static const float PHYSICAL_ANGLES[7] = {90.0, 60.0, 30.0, 0.0, -30.0, -60.0, -90.0};

        // Sorted in strictly ascending order
        static const float RAW_READINGS_LEG_1[7] = {-72.9491, -82.5220, -94.9044, -99.0382, -85.4366, -46.4964, -38.9780};
        static const float RAW_READINGS_LEG_2[7] = {151.8218, 154.4275, 157.8259, 153.6640, 153.5046, 152.9168, 150.4118};
        static const float RAW_READINGS_LEG_3[7] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    }

} // End of Params
