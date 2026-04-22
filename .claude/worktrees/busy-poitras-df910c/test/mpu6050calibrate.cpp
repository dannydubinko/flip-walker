#include "I2Cdev.h"
#include "MPU6050.h"
#include <Arduino.h>
#include <Wire.h>

MPU6050 mpu;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    Wire.setClock(400000); // Fast I2C

    Serial.println("Initializing MPU6050 for Calibration...");
    mpu.initialize();

    // 1. Connection Test
    if (!mpu.testConnection()) {
        Serial.println("MPU6050 connection failed");
        while (1);
    }
    Serial.println("MPU6050 connection successful");

    // 2. CRITICAL: FORCE SENSITIVITY
    // We must calibrate in the same mode we run the DMP in (+/- 2g)
    mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
    mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_2000);

    // 3. Reset Offsets to 0 to start fresh
    mpu.setXAccelOffset(0);
    mpu.setYAccelOffset(0);
    mpu.setZAccelOffset(0);
    mpu.setXGyroOffset(0);
    mpu.setYGyroOffset(0);
    mpu.setZGyroOffset(0);

    Serial.println("\nStarting Internal Calibration...");
    Serial.println("PLEASE KEEP SENSOR COMPLETELY STILL!");
    
    // 4. Run Auto-Calibration
    // The '6' means it will loop roughly 600-1000 times to fine-tune
    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);

    // 5. Print Results
    Serial.println("\n\n---------------------------------------");
    Serial.println("       CALIBRATION COMPLETE");
    Serial.println("---------------------------------------");
    mpu.PrintActiveOffsets();
    Serial.println("---------------------------------------");
    Serial.println("Copy the lines above into your main code's setupMPU() function.");
}

void loop() {
    // Do nothing
}
