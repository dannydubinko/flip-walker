#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include <Arduino.h>
#include <Wire.h>

MPU6050 mpu;

// --- CONFIGURATION ---
#define INTERRUPT_PIN 15  // Connect MPU6050 INT to ESP32 GPIO 15
#define OUTPUT_READABLE_YAWPITCHROLL

// --- FORWARD DECLARATIONS ---
void setupMPU();
void getDMPData();
void dmpDataReady();

// --- MPU CONTROL/STATUS ---
bool dmpReady = false;
uint16_t packetSize;
uint16_t fifoCount;
uint8_t fifoBuffer[64];
unsigned long lastPrintTime = 0;

// --- ORIENTATION DATA ---
Quaternion q;           
VectorFloat gravity;    
float ypr[3];           

// --- INTERRUPT LOGIC ---
volatile bool mpuInterrupt = false;
void IRAM_ATTR dmpDataReady() {
    mpuInterrupt = true;
}

void setup() {
    // 1. Set Baud Rate to 115200 (Matches your request)
    Serial.begin(115200); 
    
    // 2. Fast I2C (400kHz) is mandatory for DMP responsiveness
    Wire.begin();
    Wire.setClock(400000); 

    setupMPU();
}

void loop() {
    if (!dmpReady) return;

    // Trigger whenever Interrupt fires OR if there are packets waiting
    if (mpuInterrupt || mpu.getFIFOCount() >= packetSize) {
        getDMPData();
    }
}

void setupMPU() {
    Serial.println(F("Initializing MPU6050..."));
    mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT);

    if (!mpu.testConnection()) {
        Serial.println(F("MPU6050 connection failed"));
        while (1);
    }

    uint8_t devStatus = mpu.dmpInitialize();

    // --- CRITICAL FIX: FORCE SENSITIVITY ---
    // This fixes the issue where Pitch/Roll stop at 30-40 degrees.
    mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
    mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_2000);

    // --- ZERO OFFSETS ---
    // Start with 0 to prevent "bad" calibration from locking the sensor.
    mpu.setXAccelOffset(0);
    mpu.setYAccelOffset(0);
    mpu.setZAccelOffset(0);
    mpu.setXGyroOffset(0);
    mpu.setYGyroOffset(0);
    mpu.setZGyroOffset(0);

    if (devStatus == 0) {
        // --- DISABLE AUTO-CALIBRATION ---
        // Commented out to stop the code from overwriting our 0 offsets.
        // mpu.CalibrateAccel(6);
        // mpu.CalibrateGyro(6);
        
        mpu.setDMPEnabled(true);
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
        packetSize = mpu.dmpGetFIFOPacketSize();
        dmpReady = true;
        Serial.println(F("DMP Ready."));
    } else {
        Serial.print(F("DMP Init failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }
}

void getDMPData() {
    mpuInterrupt = false;
    uint8_t mpuIntStatus = mpu.getIntStatus();
    fifoCount = mpu.getFIFOCount();

    // Safety check for overflow
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        mpu.resetFIFO();
    } 
    else if (mpuIntStatus & 0x02) {
        // --- LAG KILLER ---
        // Clear buffer backlog to ensure data is instant
        while (fifoCount >= packetSize) {
            mpu.getFIFOBytes(fifoBuffer, packetSize);
            fifoCount -= packetSize;
        }

        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

        // --- NON-BLOCKING PRINT ---
        // Only print every 20ms to keep the loop fast
        if (millis() - lastPrintTime >= 20) {
            lastPrintTime = millis();
            
            // Convert Radians to Degrees
            float yaw   = ypr[0] * 180/M_PI;
            float pitch = ypr[1] * 180/M_PI;
            float roll  = ypr[2] * 180/M_PI;
            
            Serial.print("Y:"); Serial.print(yaw, 1);
            Serial.print("\tP:"); Serial.print(pitch, 1);
            Serial.print("\tR:"); Serial.println(roll, 1);
        }
    }
}
