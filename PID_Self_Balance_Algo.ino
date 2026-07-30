#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESP32Servo.h>

const char* ssid = "CMF1";  
const char* password = "122344566"; 

WebServer server(80);
Adafruit_MPU6050 mpu;
Servo right_prop;
Servo left_prop;

float Total_angle[2];
float elapsedTime, currentTime, previousTime;
float PID, pwmRight, error= 0 , previous_error = 0, pwmLeft;
float pid_p = 0, pid_i = 0, pid_d = 0;
float L_t_M= 0. ; // ye additional throttle dene ke liye variable hai wifi server se change hoga
float R_t_M= 0. ; // ye additional throttle dene ke liye variable hai wifi server se change hoga

double kp = 3.00;
double ki = 0.00;
double kd = 0.00;

double throttle = 1150;
float desired_angle = 0.00;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/update", handleUpdate);
  server.on("/throttle", handleThrottle);
  server.on("/margins", handleMargins);
  server.begin();
  Serial.println("Web server started!");

  Wire.begin();
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found! Check wiring.");
    while (1);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(100);
  right_prop.attach(8, 1000, 2000);
  left_prop.attach(10, 1000, 2000);
  
  currentTime = millis();
  Serial.print("Time: ");
  Serial.print(" , ");
  Serial.print("s | Angle: ");
  left_prop.writeMicroseconds(1000);
  right_prop.writeMicroseconds(1000);
  delay(3000);
}

void loop() {
  server.handleClient(); 
  
  previousTime = currentTime;
  currentTime = millis();
  elapsedTime = (currentTime - previousTime) / 1000.0;
  if (elapsedTime == 0) elapsedTime = 0.001;

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float AccX = atan2(a.acceleration.y, sqrt(a.acceleration.x * a.acceleration.x + a.acceleration.z * a.acceleration.z)) * (180 / PI);
  float AccY = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;

  float GyroX = g.gyro.x * 180 / PI;
  float GyroY = g.gyro.y * 180 / PI;

  Total_angle[0] = 0.98 * (Total_angle[0] + GyroX * elapsedTime) + 0.02 * AccX;
  Total_angle[1] = 0.98 * (Total_angle[1] + GyroY * elapsedTime) + 0.02 * AccY;

  error = Total_angle[1] - desired_angle;

  pid_p = kp * error;

  pid_i += ki * error;  
  

  pid_d = kd * ((error - previous_error) / elapsedTime);
  PID = pid_p + pid_i - pid_d;
  PID = constrain(PID, -1000, 1000);


  pwmLeft = max(throttle + PID + L_t_M , 1050.);
  pwmRight = max(throttle - PID + R_t_M, 1050. );
  pwmLeft = constrain(pwmLeft, 1050, 2000);
  pwmRight = constrain(pwmRight, 1050, 2000);

  Serial.print(millis() / 1000.0, 3);
  Serial.print(" , "); // 
  Serial.println(Total_angle[1]);

  left_prop.writeMicroseconds(pwmLeft);
  right_prop.writeMicroseconds(pwmRight);
  previous_error = error;
}
// This part of code (server updation of data) is done by chatgpt
void handleRoot() {
  String page = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  page += "<title>Drone Controller</title>";
  page += "<style>";
  page += "body{font-family:Arial,sans-serif;margin:10px;padding:0;background:#f5f5f5;}";
  page += "h2{color:#333;text-align:center;}";
  page += "form{margin-bottom:15px;padding:10px;background:#fff;border-radius:5px;box-shadow:0 2px 5px rgba(0,0,0,0.1);}";
  page += "h3{margin-top:0;color:#444;}";
  page += "input[type='text'],input[type='number']{width:100%;padding:8px;margin:5px 0;box-sizing:border-box;border:1px solid #ddd;border-radius:4px;}";
  page += "input[type='submit']{width:100%;padding:10px;background:#4CAF50;color:white;border:none;border-radius:4px;cursor:pointer;}";
  page += "input[type='submit']:hover{background:#45a049;}";
  page += ".status{background:#fff;padding:10px;border-radius:5px;margin-top:10px;box-shadow:0 2px 5px rgba(0,0,0,0.1);}";
  page += "</style></head>";
  page += "<body><h2>Drone Controller</h2>";
  
 
  page += "<form action='/update' method='POST'>";
  page += "<h3>PID Control</h3>";
  page += "kp: <input type='text' name='kp' value='" + String(kp) + "'><br>";
  page += "ki: <input type='text' name='ki' value='" + String(ki) + "'><br>";
  page += "kd: <input type='text' name='kd' value='" + String(kd) + "'><br>";
  page += "<input type='submit' value='Update PID'>";
  page += "</form>";
  
 
  page += "<form action='/throttle' method='POST'>";
  page += "<h3>Throttle Control</h3>";
  page += "Throttle (1000-2000): <input type='number' name='throttle' value='" + String(throttle) + "' min='1000' max='2000' step='10'><br>";
  page += "<input type='submit' value='Update Throttle'>";
  page += "</form>";
  
 
  page += "<form action='/margins' method='POST'>";
  page += "<h3>Throttle Margins</h3>";
  page += "Left Throttle Margin: <input type='number' name='ltm' value='" + String(L_t_M) + "' step='1'><br>";
  page += "Right Throttle Margin: <input type='number' name='rtm' value='" + String(R_t_M) + "' step='1'><br>";
  page += "<input type='submit' value='Update Margins'>";
  page += "</form>";
  
  page += "<div class='status'>";
  page += "<h3>Current Values</h3>";
  page += "<p>Throttle: " + String(throttle) + "</p>";
  page += "<p>Left Margin: " + String(L_t_M) + "</p>";
  page += "<p>Right Margin: " + String(R_t_M) + "</p>";
  page += "<p>PID Values - P: " + String(kp) + ", I: " + String(ki) + ", D: " + String(kd) + "</p>";
  page += "</div>";
  
  page += "</body></html>";
  server.send(200, "text/html", page);
}

void handleUpdate() {
  if (server.hasArg("kp")) kp = server.arg("kp").toDouble();
  if (server.hasArg("ki")) ki = server.arg("ki").toDouble();
  if (server.hasArg("kd")) kd = server.arg("kd").toDouble();
  
  handleRoot(); // Return to the main page with updated values
}

void handleThrottle() {
  if (server.hasArg("throttle")) {
    throttle = server.arg("throttle").toInt();
    throttle = constrain(throttle, 1000, 2000);
  }
  handleRoot(); // Return to the main page with updated values
}

void handleMargins() {
  if (server.hasArg("ltm")) L_t_M = server.arg("ltm").toFloat();
  if (server.hasArg("rtm")) R_t_M = server.arg("rtm").toFloat();
  handleRoot(); // Return to the main page with updated values
}
