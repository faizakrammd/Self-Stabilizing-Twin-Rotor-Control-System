#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <MPU6050.h>
#include <ESP32Servo.h>

// MPU & Servo
MPU6050 mpu;
Servo escLeft;
Servo escRight;

// ESC pins
const int escPinLeft = 8;
const int escPinRight = 10;

// WiFi credentials
const char* ssid = "OnePlus Nord CE 2 Lite 5G";
const char* password = "01afzal01";

// Web server
WebServer server(80);

// PID constants
float Kp = 0.0, Ki = 0.0, Kd = 0.0;

// PID variables
float desiredAngle = 0;
float angle = 0, error = 0, previousError = 0;
float integral = 0, derivative = 0;
float output = 0;

// Timing
unsigned long lastTime = 0;
float dt = 0;

// Adjustable values
int baseSpeed = 1250;
float R_margin = 0.0;
float L_margin = 0.0;

// === Additions for stability ===
#define MAX_INTEGRAL 100.0
#define MIN_INTEGRAL -100.0
float lastOutput = 0;
float alpha = 0.6;  // Smoothing factor for output

void handleRoot() {
  String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'>"
                "<style>body{font-family:Arial;padding:20px;}input{width:100%;padding:8px;margin:5px 0;}"
                "form{border:1px solid #ccc;padding:15px;border-radius:10px;}</style></head><body>"
                "<h2>Set PID and Motor Values</h2>"
                "<form method='POST' action='/setValues'>"
                "Kp: <input name='kp' value='" + String(Kp) + "'><br>"
                "Ki: <input name='ki' value='" + String(Ki) + "'><br>"
                "Kd: <input name='kd' value='" + String(Kd) + "'><br>"
                "Base Speed: <input name='baseSpeed' value='" + String(baseSpeed) + "'><br>"
                "R_margin: <input name='rmargin' value='" + String(R_margin) + "'><br>"
                "L_margin: <input name='lmargin' value='" + String(L_margin) + "'><br>"
                "<input type='submit' value='Update'></form>"
                "<p><strong>Current Values:</strong><br>"
                "Kp: " + String(Kp) + ", Ki: " + String(Ki) + ", Kd: " + String(Kd) + "<br>"
                "Base Speed: " + String(baseSpeed) + "<br>"
                "R_margin: " + String(R_margin) + "<br>"
                "L_margin: " + String(L_margin) + "</p>"
                "</body></html>";
  server.send(200, "text/html", html);
}

void handleSetValues() {
  if (server.hasArg("kp")) Kp = server.arg("kp").toFloat();
  if (server.hasArg("ki")) Ki = server.arg("ki").toFloat();
  if (server.hasArg("kd")) Kd = server.arg("kd").toFloat();
  if (server.hasArg("baseSpeed")) baseSpeed = server.arg("baseSpeed").toInt();
  if (server.hasArg("rmargin")) R_margin = server.arg("rmargin").toFloat();
  if (server.hasArg("lmargin")) L_margin = server.arg("lmargin").toFloat();
  handleRoot(); // Show updated values on the same page
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 not connected!");
    while (1);
  }

  // Attach ESCs
  escLeft.attach(escPinLeft, 1000, 2000);
  escRight.attach(escPinRight, 1000, 2000);
  escLeft.writeMicroseconds(1000);
  escRight.writeMicroseconds(1000);
  delay(3000);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected! IP: " + WiFi.localIP().toString());

  // Setup Web Server
  server.on("/", handleRoot);
  server.on("/setValues", HTTP_POST, handleSetValues);
  server.begin();

  lastTime = millis();
}

void loop() {
  server.handleClient();

  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  float accelAngle = atan2(ay, az) * 180.0 / PI;
  float gyroRate = gx / 131.0;

  unsigned long now = millis();
  dt = (now - lastTime) / 1000.0;
  if (dt <= 0) dt = 0.01; // avoid division by zero
  lastTime = now;

  angle = 0.98 * (angle + gyroRate * dt) + 0.02 * accelAngle;

  error = desiredAngle - angle;
  integral += error * dt;

  // === Clamp integral to prevent windup ===
  integral = constrain(integral, MIN_INTEGRAL, MAX_INTEGRAL);

  derivative = (error - previousError) / dt;
  float rawOutput = Kp * error + Ki * integral + Kd * derivative;
  previousError = error;

  // === Smooth PID output to reduce oscillations ===
  output = alpha * lastOutput + (1 - alpha) * rawOutput;
  lastOutput = output;

  int motorLeftSpeed = constrain(baseSpeed + output + R_margin , 1000, 2000); 
  int motorRightSpeed = constrain(baseSpeed - output + L_margin , 1000, 2000); 

  escLeft.writeMicroseconds(motorLeftSpeed + 30); // unchanged from your code
  escRight.writeMicroseconds(motorRightSpeed);

  Serial.print(millis()); Serial.print(",");
  Serial.print(angle); Serial.print(",");
  Serial.print(output); Serial.print(",");
  Serial.print(motorLeftSpeed); Serial.print(",");
  Serial.println(motorRightSpeed);
}
