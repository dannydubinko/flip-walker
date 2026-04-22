#include "Adafruit_MLX90393.h"

// 1. Create two separate sensor objects
Adafruit_MLX90393 sensor1 = Adafruit_MLX90393();
Adafruit_MLX90393 sensor2 = Adafruit_MLX90393();

void setup(void)
{
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  // 2. Initialize Sensor 1 (Default Address 0x0C)
  if (! sensor1.begin_I2C(0x0C)) {
    Serial.println("Could not find Sensor 1 (0x0C)!");
    while (1) delay(10);
  }

  // 3. Initialize Sensor 2 (Address 0x0D -> A0 connected to High)
  if (! sensor2.begin_I2C(0x0D)) {
    Serial.println("Could not find Sensor 2 (0x0D)!");
    while (1) delay(10);
  }

  Serial.println("Both sensors found!");

  // Set gain/resolution for both...
  sensor1.setGain(MLX90393_GAIN_1X);
  sensor2.setGain(MLX90393_GAIN_1X);
}

void loop(void) {
  float x1, y1, z1;
  float x2, y2, z2;

  // Read Sensor 1
  if(sensor1.readData(&x1, &y1, &z1)) {
     Serial.print("Sensor 1 X: "); Serial.println(x1);
  }

  // Read Sensor 2
  if(sensor2.readData(&x2, &y2, &z2)) {
     Serial.print("Sensor 2 X: "); Serial.println(x2);
  }

  delay(100);
}
