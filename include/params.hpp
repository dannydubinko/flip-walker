#pragma once
#include <string>

namespace Params
{
    constexpr int serialPort{115200};
    constexpr int magnetometerAddr1{0x18};

    namespace Pins
    {
        // Servo
        static const int servoPin1{13};
        static const int servoPin2{12};

        // DC Motors
        constexpr int kMotor1_In1{27};
        constexpr int kMotor1_In2{14};
        constexpr int kMotor2_In1{26};
        constexpr int kMotor2_In2{25};
    }

    namespace Wifi
    {
        const char *wifi_name{"BELL549"};
        const char *wifi_password{"1FFCC99A399E"};

        constexpr int portNumber{80};
    }

    // // Controller Gains
    // namespace PID {
    //     constexpr double kP = 15.0;
    //     constexpr double kI = 0.05;
    //     constexpr double kD = 1.2;
    // }

    // // Hardware Config
    // const std::string kRobotID = "FLIP_WALKER_01";
}
