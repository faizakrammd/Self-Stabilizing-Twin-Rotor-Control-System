#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <ESP32Servo.h> 
Adafruit_MPU6050 IMU1; 

unsigned long startTime1;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  Wire.begin();

  if (!IMU1.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  IMU1.setAccelerometerRange(MPU6050_RANGE_8_G);
  IMU1.setGyroRange(MPU6050_RANGE_500_DEG);
  IMU1.setFilterBandwidth(MPU6050_BAND_21_HZ);
  startTime1 = millis();
  Serial.println("Time (ms),Angle (degrees)");
}

void loop() {

  sensors_event_t a, g, temp;
  IMU1.getEvent(&a, &g, &temp);

  float roll = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;
  unsigned long elapsedTime = millis() - startTime1;
  Serial.print(elapsedTime);
  Serial.print(",");
  Serial.println(roll, 2); 


  delay(100); 
}
