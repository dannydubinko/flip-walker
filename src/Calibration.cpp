// #include <Arduino.h>
// #include "Magnetometer.hpp"
// #include "MagnetometerCalibrator.hpp"
// #include "params.hpp"

// // Initialize the sensor at the I2C address (0x0C is default for MLX90393)
// Magnetometer mag(Params::magnetometerAddr1);
// MagnetometerCalibrator calibrator(mag);

// #define MOTOR1_IN1 27
// #define MOTOR1_IN2 14
// #define MOTOR2_IN1 26
// #define MOTOR2_IN2 25

// void setMotorSpeed(int motor, int speed)
// {
//   int in1 = (motor == 1) ? MOTOR1_IN1 : MOTOR2_IN1;
//   int in2 = (motor == 1) ? MOTOR1_IN2 : MOTOR2_IN2;

//   analogWrite(in1, max(speed, 0));
//   analogWrite(in2, max(-speed, 0));
// }

// void setup()
// {
//   Serial.begin(115200);

//   pinMode(MOTOR1_IN1, OUTPUT);
//   pinMode(MOTOR1_IN2, OUTPUT);
//   pinMode(MOTOR2_IN1, OUTPUT);
//   pinMode(MOTOR2_IN2, OUTPUT);

//   setMotorSpeed(1, 0);
//   setMotorSpeed(2, 0);

//   while (!Serial)
//     delay(10); // Wait for Serial Monitor to open

//   Serial.println("\n========================================");
//   Serial.println("   FlipWalker Magnetometer Calibrator   ");
//   Serial.println("========================================\n");

//   // Initialize sensor with specific gain/res for high precision
//   // Gain 1x or 2x is usually best for strong Neodymium magnets
//   mag.initalize(MLX90393_GAIN_1X, MLX90393_RES_16, MLX90393_OSR_3, MLX90393_FILTER_5);

//   Serial.println("Ready to start?");
//   Serial.println("Type anything and press Enter to begin...");

//   // Wait for user to be ready
//   while (Serial.available() == 0)
//     delay(10);
//   while (Serial.available() > 0)
//     Serial.read();

//   // Run the 13-point calibration (-90 to 90 in 15-degree increments)
//   calibrator.runInterativeCalibration(7);

//   Serial.println("\nCalibration Finished!");
//   Serial.println("1. Copy the arrays above.");
//   Serial.println("2. Paste them into your production code.");
//   Serial.println("3. Re-flash your FlipWalker main firmware.");
// }

// void loop()
// {
// }
