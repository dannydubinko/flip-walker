#include "Adafruit_MLX90393.h"
#include <Arduino.h>


Adafruit_MLX90393 sensor = Adafruit_MLX90393();
#define MLX90393_CS 10

void setup(void)
{
  Serial.begin(115200);

  /* Wait for serial on USB platforms. */
  while (!Serial) {
      delay(10);
  }

  Serial.println("Starting Adafruit MLX90393 Demo");

  if (! sensor.begin_I2C(0x18)) {          // hardware I2C mode, can pass in address & alt Wire
  //if (! sensor.begin_SPI(MLX90393_CS)) {  // hardware SPI mode
    Serial.println("No sensor found ... check your wiring?");
    while (1) { delay(10); }
  }
  Serial.println("Found a MLX90393 sensor");

  sensor.setGain(MLX90393_GAIN_1X);
  // You can check the gain too
  Serial.print("Gain set to: ");
  switch (sensor.getGain()) {
    case MLX90393_GAIN_1X: Serial.println("1 x"); break;
    case MLX90393_GAIN_1_33X: Serial.println("1.33 x"); break;
    case MLX90393_GAIN_1_67X: Serial.println("1.67 x"); break;
    case MLX90393_GAIN_2X: Serial.println("2 x"); break;
    case MLX90393_GAIN_2_5X: Serial.println("2.5 x"); break;
    case MLX90393_GAIN_3X: Serial.println("3 x"); break;
    case MLX90393_GAIN_4X: Serial.println("4 x"); break;
    case MLX90393_GAIN_5X: Serial.println("5 x"); break;
  }

    // 1. Lower the Resolution slightly (Speed up processing)
    // (Change RES_17/16 to RES_16 for all)
    sensor.setResolution(MLX90393_X, MLX90393_RES_16);
    sensor.setResolution(MLX90393_Y, MLX90393_RES_16);
    sensor.setResolution(MLX90393_Z, MLX90393_RES_16);

    // 2. Reduce Oversampling (Don't average so much)
    // Change OSR_3 to OSR_0 or OSR_1
    sensor.setOversampling(MLX90393_OSR_0);

    // 3. Lower the Filter (The most important fix)
    // Change FILTER_5 to FILTER_1 or FILTER_0
    sensor.setFilter(MLX90393_FILTER_0);
}

void loop(void) {
  float x, y, z;

  double K = 6185184.1;

//   // get X Y and Z data at once
//   if (sensor.readData(&x, &y, &z)) {
//     //   Serial.print("X: "); Serial.print(x, 4); Serial.println(" uT");
//     //   Serial.print("Y: "); Serial.print(y, 4); Serial.println(" uT");
//     //   Serial.print("Z: "); Serial.print(z, 4); Serial.println(" uT");
//         float magnitude = sqrt(sq(x) + sq(y) + sq(z));
//         Serial.print("Mag: "); Serial.print(magnitude, 4); Serial.println("");

//         double distance = cbrt(K/z);
//         Serial.print("Dis: "); Serial.print(distance, 4); Serial.println(" mm");
//   } else {
//       Serial.println("Unable to read XYZ data from the sensor.");
//   }

//   delay(25);

  if (sensor.readData(&x, &y, &z)) {
      // 1. Calculate the angle in Radians
      // We swap Y and X because of how the sensor grid is laid out
      float heading = atan2(x, z);
      
      // 2. Convert Radians to Degrees
      float headingDegrees = (heading) * 180.0 / PI;
      
      // 3. Fix negative angles (so -90 becomes 270)
    //   if (headingDegrees < 0) {
    //     headingDegrees += 360;
    //   }
      
      Serial.print("Angle: "); Serial.println(headingDegrees);
    }

//   /* Or....get a new sensor event, normalized to uTesla */
//   sensors_event_t event;
//   sensor.getEvent(&event);
//   /* Display the results (magnetic field is measured in uTesla) */
//   Serial.print("X: "); Serial.print(event.magnetic.x);
//   Serial.print(" \tY: "); Serial.print(event.magnetic.y);
//   Serial.print(" \tZ: "); Serial.print(event.magnetic.z);
//   Serial.println(" uTesla ");

//   delay(500);
}
