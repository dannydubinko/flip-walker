#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include "Magnetometer.hpp"
#include "params.hpp"

// States
// cBp = child behind parent  -> sensor 1 (0x18) detecting
// cAp = child ahead parent   -> sensor 2 (0x1A) detecting
enum State { cBp, cAp, UNKNOWN };

// --- Wi-Fi ---
const char *ssid     = Params::Wifi::wifi_name;
const char *password = Params::Wifi::wifi_password;

WebServer server(Params::Wifi::portNumber);

// --- Hardware ---
Servo parentServo;
Servo childServo;
Magnetometer mag1(Params::magnetometerAddr1); // 0x18
Magnetometer mag2(Params::magnetometerAddr2); // 0x1A

// --- HTML Page ---
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>FlipWalker Control</title>
  <style>
    body { font-family: 'Segoe UI', sans-serif; text-align: center; background: #f0f2f5; margin: 0; padding: 20px; }
    .card { background: white; padding: 20px; border-radius: 12px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); max-width: 420px; margin: 0 auto 20px auto; }
    h2 { color: #1a1a1a; margin-bottom: 20px; }
    h3 { color: #333; margin-top: 0; border-bottom: 2px solid #eee; padding-bottom: 10px; }
    input[type=range] { width: 100%; margin: 10px 0; }
    button { background: #007bff; color: white; border: none; padding: 12px 20px; border-radius: 8px; cursor: pointer; font-size: 15px; margin: 4px; transition: 0.2s; }
    button:hover { background: #0056b3; }
    button.danger { background: #dc3545; }
    button.danger:hover { background: #a71d2a; }
    button.success { background: #28a745; }
    button.success:hover { background: #1a6b2e; }
    .val { font-weight: bold; color: #007bff; }
    .state-box { font-size: 1.2em; font-weight: bold; padding: 10px; border-radius: 8px; background: #f8f9fa; margin-top: 10px; }
    .sensor-row { display: flex; justify-content: space-around; margin-top: 12px; }
    .sensor-box { background: #f8f9fa; padding: 10px; border-radius: 8px; width: 45%; }
    .present { color: #28a745; font-weight: bold; }
    .absent  { color: #dc3545; font-weight: bold; }
  </style>
</head>
<body>
  <h2>FlipWalker Control</h2>

  <div class="card">
    <h3>Robot State</h3>
    <div class="state-box" id="state">--</div>
    <br>
    <button class="success" onclick="sendCmd('flip')">FLIP</button>
    <button onclick="fetchState()">Refresh State</button>
  </div>

  <div class="card">
    <h3>Sensors</h3>
    <button onclick="fetchSensors()">Read Sensors</button>
    <div class="sensor-row">
      <div class="sensor-box">
        Mag1 (0x18)<br>
        <span id="mag1_mag">-</span><br>
        <span id="mag1_state">-</span>
      </div>
      <div class="sensor-box">
        Mag2 (0x1A)<br>
        <span id="mag2_mag">-</span><br>
        <span id="mag2_state">-</span>
      </div>
    </div>
  </div>

  <div class="card">
    <h3>Servos</h3>
    <p>Parent: <span id="p_val" class="val">90</span>&deg;</p>
    <input type="range" min="0" max="180" value="90"
      oninput="document.getElementById('p_val').innerText=this.value"
      onchange="sendCmd('p'+this.value)">
    <p>Child: <span id="c_val" class="val">90</span>&deg;</p>
    <input type="range" min="0" max="180" value="90"
      oninput="document.getElementById('c_val').innerText=this.value"
      onchange="sendCmd('c'+this.value)">
  </div>

  <div class="card">
    <h3>Drive</h3>
    <div style="display:grid;grid-template-columns:repeat(3,1fr);gap:8px;max-width:240px;margin:0 auto;">
      <div></div>
      <button onclick="sendCmd('fwd')">&#9650; Fwd</button>
      <div></div>
      <button onclick="sendCmd('left')">&#9664; Left</button>
      <button class="danger" onclick="sendCmd('stop')">Stop</button>
      <button onclick="sendCmd('right')">Right &#9654;</button>
      <div></div>
      <button onclick="sendCmd('bwd')">&#9660; Bwd</button>
      <div></div>
    </div>
  </div>

  <div class="card">
    <h3>DC Motors</h3>
    <p>Motor 1: <span id="m1_val" class="val">0</span></p>
    <input type="range" min="-255" max="255" value="0"
      oninput="document.getElementById('m1_val').innerText=this.value"
      onchange="sendCmd('m1'+this.value)">
    <p>Motor 2: <span id="m2_val" class="val">0</span></p>
    <input type="range" min="-255" max="255" value="0"
      oninput="document.getElementById('m2_val').innerText=this.value"
      onchange="sendCmd('m2'+this.value)">
    <br>
    <button class="danger" onclick="sendCmd('stop')">STOP MOTORS</button>
  </div>

  <script>
    function sendCmd(cmd) {
      fetch('/cmd?v=' + encodeURIComponent(cmd))
        .then(r => r.json())
        .then(d => {
          if (d.state) document.getElementById('state').innerText = d.state;
        });
    }

    function fetchState() {
      fetch('/cmd?v=state')
        .then(r => r.json())
        .then(d => { document.getElementById('state').innerText = d.state || '--'; });
    }

    function fetchSensors() {
      fetch('/sensors')
        .then(r => r.json())
        .then(d => {
          document.getElementById('mag1_mag').innerText   = d.mag1_mag.toFixed(1);
          document.getElementById('mag1_state').innerHTML = d.mag1_present
            ? '<span class="present">PRESENT</span>' : '<span class="absent">NONE</span>';
          document.getElementById('mag2_mag').innerText   = d.mag2_mag.toFixed(1);
          document.getElementById('mag2_state').innerHTML = d.mag2_present
            ? '<span class="present">PRESENT</span>' : '<span class="absent">NONE</span>';
        });
    }

    fetchState();
    fetchSensors();
  </script>
</body>
</html>
)rawliteral";

// --- Motor ---
void setMotorSpeed(int motor, int speed)
{
    int in1 = (motor == 1) ? Params::Pins::kMotor1_In1 : Params::Pins::kMotor2_In1;
    int in2 = (motor == 1) ? Params::Pins::kMotor1_In2 : Params::Pins::kMotor2_In2;
    speed = constrain(speed, -255, 255);
    if (speed >= 0) { analogWrite(in1, speed); analogWrite(in2, 0); }
    else            { analogWrite(in1, 0);     analogWrite(in2, -speed); }
}

void driveForward()
{
    setMotorSpeed(Params::Motors::leftMotor,  Params::Motors::leftForward  * Params::Motors::driveSpeed);
    setMotorSpeed(Params::Motors::rightMotor, Params::Motors::rightForward * Params::Motors::driveSpeed);
}

void driveBackward()
{
    setMotorSpeed(Params::Motors::leftMotor,  -Params::Motors::leftForward  * Params::Motors::driveSpeed);
    setMotorSpeed(Params::Motors::rightMotor, -Params::Motors::rightForward * Params::Motors::driveSpeed);
}

void turnLeft()
{
    setMotorSpeed(Params::Motors::leftMotor,  -Params::Motors::leftForward  * Params::Motors::driveSpeed);
    setMotorSpeed(Params::Motors::rightMotor,  Params::Motors::rightForward * Params::Motors::driveSpeed);
}

void turnRight()
{
    setMotorSpeed(Params::Motors::leftMotor,   Params::Motors::leftForward  * Params::Motors::driveSpeed);
    setMotorSpeed(Params::Motors::rightMotor, -Params::Motors::rightForward * Params::Motors::driveSpeed);
}

// --- State Machine ---
State getState()
{
    bool s1 = mag1.isMagnetPresent();
    bool s2 = mag2.isMagnetPresent();
    if (s1 && !s2) return cBp;
    if (s2 && !s1) return cAp;
    return UNKNOWN;
}

const char* stateName(State s)
{
    if (s == cBp) return "cBp (child behind parent)";
    if (s == cAp) return "cAp (child ahead parent)";
    return "UNKNOWN";
}

bool waitForTransition()
{
    unsigned long start = millis();
    while (mag1.isMagnetPresent() || mag2.isMagnetPresent())
    {
        if (millis() - start > 5000)
        {
            Serial.println("WARNING: Transition timeout — sensors still detecting.");
            return false;
        }
        delay(50);
    }
    return true;
}

void doFlip()
{
    State s = getState();

    if (s == cBp)
    {
        Serial.println("cBp -> flip forward: child servo to 0...");
        childServo.write(0);
        delay(1500);

        Serial.println("Waiting for mid-transition (both sensors clear)...");
        waitForTransition();

        Serial.println("Mid-point reached. Returning child servo to 90...");
        childServo.write(90);
        delay(2000);

        Serial.println("Activating parent servo to 0...");
        parentServo.write(0);
        delay(1500);
        parentServo.write(90);
        delay(1500);
    }
    else if (s == cAp)
    {
        Serial.println("cAp -> flip forward: parent servo to 180...");
        parentServo.write(180);
        delay(1500);

        Serial.println("Waiting for mid-transition (both sensors clear)...");
        waitForTransition();

        Serial.println("Mid-point reached. Returning parent servo to 90...");
        parentServo.write(90);
        delay(2000);

        Serial.println("Activating child servo to 180...");
        childServo.write(180);
        delay(1500);
        childServo.write(90);
        delay(1500);
    }
    else
    {
        Serial.println("Cannot flip — state is UNKNOWN.");
    }
}

// --- Web Routes ---
void handleCmd()
{
    String cmd = server.arg("v");
    cmd.trim();
    String responseState = stateName(getState());

    if (cmd.equalsIgnoreCase("flip"))
    {
        doFlip();
        responseState = stateName(getState());
    }
    else if (cmd.equalsIgnoreCase("state"))
    {
        // just return state
    }
    else if (cmd.equalsIgnoreCase("r"))
    {
        // handled via /sensors
    }
    else if (cmd.equalsIgnoreCase("fwd"))
    {
        driveForward();
    }
    else if (cmd.equalsIgnoreCase("bwd"))
    {
        driveBackward();
    }
    else if (cmd.equalsIgnoreCase("left"))
    {
        turnLeft();
    }
    else if (cmd.equalsIgnoreCase("right"))
    {
        turnRight();
    }
    else if (cmd.equalsIgnoreCase("stop"))
    {
        setMotorSpeed(1, 0);
        setMotorSpeed(2, 0);
    }
    else if (cmd.length() > 1 && cmd.charAt(0) == 'p')
    {
        int angle = constrain(cmd.substring(1).toInt(), 0, 180);
        parentServo.write(angle);
    }
    else if (cmd.length() > 1 && cmd.charAt(0) == 'c')
    {
        int angle = constrain(cmd.substring(1).toInt(), 0, 180);
        childServo.write(angle);
    }
    else if (cmd.length() > 2 && cmd.startsWith("m1"))
    {
        setMotorSpeed(1, cmd.substring(2).toInt());
    }
    else if (cmd.length() > 2 && cmd.startsWith("m2"))
    {
        setMotorSpeed(2, cmd.substring(2).toInt());
    }

    String json = "{\"state\":\"" + responseState + "\"}";
    server.send(200, "application/json", json);
}

void handleSensors()
{
    String json = "{";
    json += "\"mag1_mag\":"     + String(mag1.getMagnitude(), 2) + ",";
    json += "\"mag1_present\":" + String(mag1.isMagnetPresent() ? "true" : "false") + ",";
    json += "\"mag2_mag\":"     + String(mag2.getMagnitude(), 2) + ",";
    json += "\"mag2_present\":" + String(mag2.isMagnetPresent() ? "true" : "false");
    json += "}";
    server.send(200, "application/json", json);
}

// --- Setup & Loop ---
void setup()
{
    Serial.begin(115200);

    pinMode(Params::Pins::kMotor1_In1, OUTPUT); pinMode(Params::Pins::kMotor1_In2, OUTPUT);
    pinMode(Params::Pins::kMotor2_In1, OUTPUT); pinMode(Params::Pins::kMotor2_In2, OUTPUT);
    setMotorSpeed(1, 0);
    setMotorSpeed(2, 0);

    parentServo.attach(Params::Pins::servoPin1);
    childServo.attach(Params::Pins::servoPin2);
    parentServo.write(90);
    childServo.write(90);

    mag1.initalize();
    mag2.initalize();
    mag1.setFullCalibration(
        Params::MagSensorValues::RAW_READINGS_LEG_1,
        Params::MagSensorValues::PHYSICAL_ANGLES);
    mag2.setFullCalibration(
        Params::MagSensorValues::RAW_READINGS_LEG_2,
        Params::MagSensorValues::PHYSICAL_ANGLES);

    delay(5000);
    Serial.print("Connecting to Wi-Fi: "); Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
    Serial.println("\nWi-Fi Connected! IP: ");
    Serial.println(WiFi.localIP());

    server.on("/",        HTTP_GET, []() { server.send(200, "text/html", index_html); });
    server.on("/cmd",     HTTP_GET, handleCmd);
    server.on("/sensors", HTTP_GET, handleSensors);
    server.begin();
    Serial.println("HTTP Server Started.");
}

void loop()
{
    server.handleClient();
}
