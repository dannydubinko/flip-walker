// #include <Arduino.h>
// #include "Magnetometer.hpp"
// #include "MagnetometerCalibrator.hpp"
// #include "params.hpp"

// // Sensor 1 = 0x18 = magnetometerAddr1 = LEG_1
// // Sensor 2 = 0x1A = magnetometerAddr2 = LEG_2

// Magnetometer mag1(Params::magnetometerAddr1); // 0x18
// Magnetometer mag2(Params::magnetometerAddr2); // 0x1A

// MagnetometerCalibrator calibrator1(mag1);
// MagnetometerCalibrator calibrator2(mag2);

// void setup()
// {
//     Serial.begin(115200);
//     while (!Serial) delay(10);

//     // Stop motors immediately
//     pinMode(Params::Pins::kMotor1_In1, OUTPUT); pinMode(Params::Pins::kMotor1_In2, OUTPUT);
//     pinMode(Params::Pins::kMotor2_In1, OUTPUT); pinMode(Params::Pins::kMotor2_In2, OUTPUT);
//     analogWrite(Params::Pins::kMotor1_In1, 0);  analogWrite(Params::Pins::kMotor1_In2, 0);
//     analogWrite(Params::Pins::kMotor2_In1, 0);  analogWrite(Params::Pins::kMotor2_In2, 0);

//     Serial.println("\n========================================");
//     Serial.println("   FlipWalker Magnetometer Calibrator   ");
//     Serial.println("========================================");
//     Serial.println("Press any key to start...");
//     while (Serial.available() == 0) delay(10);
//     while (Serial.available() > 0) Serial.read();

//     mag1.initalize();
//     mag2.initalize();

//     Serial.println("\nAvailable sensors:");
//     Serial.print("  1 - Sensor 1  (0x18 = magnetometerAddr1)  magnitude: ");
//     Serial.println(mag1.getMagnitude(), 2);
//     Serial.print("  2 - Sensor 2  (0x1A = magnetometerAddr2)  magnitude: ");
//     Serial.println(mag2.getMagnitude(), 2);
//     Serial.print("  (magnet threshold: ");
//     Serial.print(Params::MagSensorValues::minMagnitude, 0);
//     Serial.println(")");
//     Serial.println("\nType 1 or 2 to choose which sensor to calibrate first, then press Enter:");

//     while (Serial.available() == 0) delay(10);
//     int choice = Serial.parseInt();
//     while (Serial.available() > 0) Serial.read();

//     if (choice != 1 && choice != 2)
//     {
//         Serial.println("Invalid choice. Defaulting to Sensor 1 first.");
//         choice = 1;
//     }

//     int first  = choice;
//     int second = (choice == 1) ? 2 : 1;

//     auto calibrateFirst  = (first  == 1) ? &calibrator1 : &calibrator2;
//     auto calibrateSecond = (second == 1) ? &calibrator1 : &calibrator2;
//     String labelFirst    = (first  == 1) ? "LEG_1" : "LEG_2";
//     String labelSecond   = (second == 1) ? "LEG_1" : "LEG_2";

//     Serial.print("\nStarting with Sensor "); Serial.print(first);
//     Serial.println(first == 1 ? " (0x18)" : " (0x1A)");
//     Serial.println("At each step, move the joint to the target angle and press any key.");
//     Serial.println("Press any key to begin...");
//     while (Serial.available() == 0) delay(10);
//     while (Serial.available() > 0) Serial.read();

//     calibrateFirst->runInterativeCalibration(7, labelFirst);

//     Serial.print("\n--- Sensor "); Serial.print(first); Serial.print(" done.");
//     Serial.print(" Now calibrating Sensor "); Serial.print(second);
//     Serial.println(second == 1 ? " (0x18). Press any key to begin... ---" : " (0x1A). Press any key to begin... ---");
//     while (Serial.available() == 0) delay(10);
//     while (Serial.available() > 0) Serial.read();

//     calibrateSecond->runInterativeCalibration(7, labelSecond);

//     Serial.println("\n========================================");
//     Serial.println("Both sensors calibrated!");
//     Serial.println("Paste the arrays above into params.hpp under MagSensorValues.");
//     Serial.println("========================================");
// }

// void loop() {}
