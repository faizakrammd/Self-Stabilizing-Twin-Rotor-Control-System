#include <ESP32Servo.h>

Servo escLeft;
Servo escRight;

void setup() {
  Serial.begin(115200);

  escLeft.attach(8);  
  escRight.attach(10); 

  Serial.println("Sending MAX throttle (2000µs) to both ESCs...");
  escLeft.writeMicroseconds(2000);
  escRight.writeMicroseconds(2000);
  delay(3000); 

  Serial.println("Now sending MIN throttle (1000µs)...");
  escLeft.writeMicroseconds(1000);
  escRight.writeMicroseconds(1000);
  delay(3000);  

  Serial.println("Calibration complete. You can now power-cycle the ESCs.");
}

void loop() {
  // Do nothing
}
