#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include "Magnetometer.hpp"
#include "params.hpp"

// --- Wi-Fi Credentials ---
const char *ssid = "Nicol 116";
const char *password = "thenerdlab";

// --- Web Server on Port 80 ---
WebServer server(80);

// --- Pin Definitions ---
// DC Motor 1 (Right / Battery Side)
#define MOTOR1_IN1 27
#define MOTOR1_IN2 14
// DC Motor 2 (Left)
#define MOTOR2_IN1 26
#define MOTOR2_IN2 25

// Servo Pins
static const int servoPin1 = 13; // Main body servo
static const int servoPin2 = 12; // Sub body servo

// --- Object Initialization ---
Servo myservo1;
Servo myservo2;
Magnetometer mag1(Params::magnetometerAddr1);
Magnetometer mag2(Params::magnetometerAddr2);

// --- HTML Web Page ---
// We use PROGMEM to store this long string in flash memory instead of RAM
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 Robot Controller</title>
  <style>
    body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; text-align: center; background-color: #f0f2f5; margin: 0; padding: 20px; }
    .card { background: white; padding: 20px; border-radius: 12px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); max-width: 400px; margin: 0 auto 20px auto; }
    h2 { color: #1a1a1a; margin-bottom: 20px; }
    h3 { color: #333; margin-top: 0; border-bottom: 2px solid #eee; padding-bottom: 10px;}
    input[type=range] { width: 100%; margin: 15px 0; }
    button { background-color: #007bff; color: white; border: none; padding: 12px 24px; border-radius: 8px; cursor: pointer; font-size: 16px; transition: 0.3s;}
    button:hover { background-color: #0056b3; }
    .val { font-weight: bold; color: #007bff; font-size: 1.1em; }
    .sensor-box { display: flex; justify-content: space-around; margin-top: 15px; }
    .sensor-readout { background: #f8f9fa; padding: 10px; border-radius: 8px; width: 40%; }
  </style>
</head>
<body>
  <h2>ESP32 Robot Control</h2>

  <div class="card">
    <h3>DC Motors</h3>
    <p>Right Motor (M1): <span id="m1_val" class="val">0</span></p>
    <input type="range" min="-255" max="255" value="0" onchange="updateMotor(1, this.value)" oninput="document.getElementById('m1_val').innerText = this.value">
    
    <p>Left Motor (M2): <span id="m2_val" class="val">0</span></p>
    <input type="range" min="-255" max="255" value="0" onchange="updateMotor(2, this.value)" oninput="document.getElementById('m2_val').innerText = this.value">
  </div>

  <div class="card">
    <h3>Servos</h3>
    <p>Main Body (S1): <span id="s1_val" class="val">90</span>&deg;</p>
    <input type="range" min="0" max="180" value="90" onchange="updateServo(1, this.value)" oninput="document.getElementById('s1_val').innerText = this.value">
    
    <p>Sub Body (S2): <span id="s2_val" class="val">90</span>&deg;</p>
    <input type="range" min="0" max="180" value="90" onchange="updateServo(2, this.value)" oninput="document.getElementById('s2_val').innerText = this.value">
  </div>

  <div class="card">
    <h3>Magnetometers</h3>
    <button onclick="readSensors()">Read Sensors Now</button>
    <div class="sensor-box">
      <div class="sensor-readout">Mag 1<br><span id="mag1" class="val">-</span>&deg;</div>
      <div class="sensor-readout">Mag 2<br><span id="mag2" class="val">-</span>&deg;</div>
    </div>
  </div>

  <script>
    function updateMotor(motor, speed) {
      fetch(`/motor?m=${motor}&v=${speed}`);
    }
    
    function updateServo(servo, angle) {
      fetch(`/servo?s=${servo}&v=${angle}`);
    }
    
    function readSensors() {
      document.getElementById('mag1').innerText = "...";
      document.getElementById('mag2').innerText = "...";
      fetch('/sensors')
        .then(response => response.json())
        .then(data => {
          document.getElementById('mag1').innerText = data.mag1.toFixed(2);
          document.getElementById('mag2').innerText = data.mag2.toFixed(2);
        })
        .catch(error => console.error('Error fetching sensor data:', error));
    }
  </script>
</body>
</html>
)rawliteral";

/**
 * Sets the speed and direction for the DC motors.
 * @param motor: 1 for Right, 2 for Left
 * @param speed: -255 to 255 (Positive = Forward, Negative = Reverse)
 */
void setMotorSpeed(int motor, int speed)
{
  int in1 = (motor == 1) ? MOTOR1_IN1 : MOTOR2_IN1;
  int in2 = (motor == 1) ? MOTOR1_IN2 : MOTOR2_IN2;

  if (speed >= 0)
  {
    analogWrite(in1, speed);
    analogWrite(in2, 0);
  }
  else
  {
    analogWrite(in1, 0);
    analogWrite(in2, -speed);
  }
}

void setup()
{
  Serial.begin(115200);

  // 1. Initialize DC Motor Pins
  pinMode(MOTOR1_IN1, OUTPUT);
  pinMode(MOTOR1_IN2, OUTPUT);
  pinMode(MOTOR2_IN1, OUTPUT);
  pinMode(MOTOR2_IN2, OUTPUT);

  // 2. Initialize Servos
  myservo1.attach(servoPin1);
  myservo2.attach(servoPin2);
  myservo1.write(90);
  myservo2.write(90);

  // 3. Initialize Magnetometer Sensor
  mag1.initalize();
  mag2.initalize();

  mag1.setFullCalibration(
      Params::MagSensorValues::RAW_READINGS_LEG_1,
      Params::MagSensorValues::PHYSICAL_ANGLES);
  mag2.setFullCalibration(
      Params::MagSensorValues::RAW_READINGS_LEG_1,
      Params::MagSensorValues::PHYSICAL_ANGLES);

  // 4. Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // --- 5. Define Web Server Routes ---

  // Serve the HTML page
  server.on("/", HTTP_GET, []()
            { server.send(200, "text/html", index_html); });

  // Handle Motor Updates (e.g. /motor?m=1&v=150)
  server.on("/motor", HTTP_GET, []()
            {
        if (server.hasArg("m") && server.hasArg("v")) {
            int m = server.arg("m").toInt();
            int v = server.arg("v").toInt();
            setMotorSpeed(m, v);
            Serial.printf("Web Command -> Motor %d set to %d\n", m, v);
        }
        server.send(200, "text/plain", "OK"); });

  // Handle Servo Updates (e.g. /servo?s=1&v=90)
  server.on("/servo", HTTP_GET, []()
            {
        if (server.hasArg("s") && server.hasArg("v")) {
            int s = server.arg("s").toInt();
            int v = server.arg("v").toInt();
            if (s == 1) myservo1.write(v);
            if (s == 2) myservo2.write(v);
            Serial.printf("Web Command -> Servo %d set to %d\n", s, v);
        }
        server.send(200, "text/plain", "OK"); });

  // Handle Sensor Readings
  server.on("/sensors", HTTP_GET, []()
            {
        float m1_val = mag1.getRawHeading();
        float m2_val = mag2.getRawHeading();
        
        // Construct JSON response
        String json = "{\"mag1\":" + String(m1_val, 2) + ", \"mag2\":" + String(m2_val, 2) + "}";
        server.send(200, "application/json", json);
        Serial.println("Web Command -> Read Sensors"); });

  // Start the server
  server.begin();
  Serial.println("HTTP Server Started.");
  Serial.println("You can still enter a servo angle (0-180) in the Serial Monitor.");
}

void loop()
{
  // 1. Listen for incoming web requests
  server.handleClient();

  // 2. Serial Input for Servo Control (Kept for debugging)
  if (Serial.available() > 0)
  {
    int angle = Serial.parseInt();

    if (angle >= 0 && angle <= 180)
    {
      Serial.print("Serial Command -> Moving Servos to: ");
      Serial.println(angle);
      myservo1.write(angle);
      myservo2.write(angle);
    }
  }
}
