#include <WiFi.h>
#include <ESP32Servo.h>
#include <Arduino.h>
#include "password.hpp"

// Create two servo objects
Servo myservo1;
Servo myservo2;

// GPIO pin definitions (for servo and DC motors)
static const int servoPin1 = 13;
static const int servoPin2 = 12;
#define MOTOR1_IN1 27
#define MOTOR1_IN2 14
#define MOTOR2_IN1 26
#define MOTOR2_IN2 25

// Replace with your Wi-Fi credentials
const char *ssid = wifi_name;
const char *password = wifi_password;

// Set web server port number to 80
WiFiServer server(80);

// Variable to store HTTP request
String header;

// Servo and motor control variables
String valueString1 = String(5);
String valueString2 = String(5);
int pos1 = 0;
int pos2 = 0;
int motor1Speed = 0; // -255 to 255 (negative means reverse)
int motor2Speed = 0; // -255 to 255

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time (in milliseconds)
const long timeoutTime = 2000;

void setup()
{
  Serial.begin(115200);

  // Attach servo objects to the specified pins
  myservo1.attach(servoPin1);
  myservo2.attach(servoPin2);

  // Set motor control pins as outputs
  pinMode(MOTOR1_IN1, OUTPUT);
  pinMode(MOTOR1_IN2, OUTPUT);
  pinMode(MOTOR2_IN1, OUTPUT);
  pinMode(MOTOR2_IN2, OUTPUT);

  // Connect to Wi-Fi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start the web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void setMotorSpeed(int motor, int speed)
{
  if (motor == 1)
  {
    if (speed > 0)
    {
      analogWrite(MOTOR1_IN1, speed);
      analogWrite(MOTOR1_IN2, 0);
    }
    else
    {
      analogWrite(MOTOR1_IN1, 0);
      analogWrite(MOTOR1_IN2, -speed);
    }
  }
  else if (motor == 2)
  {
    if (speed > 0)
    {
      analogWrite(MOTOR2_IN1, speed);
      analogWrite(MOTOR2_IN2, 0);
    }
    else
    {
      analogWrite(MOTOR2_IN1, 0);
      analogWrite(MOTOR2_IN2, -speed);
    }
  }
}

void loop()
{
  WiFiClient client = server.available(); // Listen for incoming clients

  if (client)
  { // If a new client connects
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client."); // Print message to Serial Monitor
    String currentLine = "";       // Create string to hold incoming data from client
    while (client.connected() && currentTime - previousTime <= timeoutTime)
    { // Loop while client is connected
      currentTime = millis();
      if (client.available())
      {                         // If there’s bytes to read from the client
        char c = client.read(); // Read a byte
        Serial.write(c);        // Print it to Serial Monitor
        header += c;
        if (c == '\n')
        { // If the byte is a newline character
          if (currentLine.length() == 0)
          {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // HTML Web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<style>body { font-family: Arial; text-align: center; }</style></head>");
            client.println("<body><h1>ESP32 Servo and Motor Control</h1>");

            // Servo control interface
            client.println("<p>Servo 1 Position: <span id=\"servoPos1\"></span></p>");
            client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider1\" oninput=\"updateServo(1, this.value)\">");
            client.println("<p>Servo 2 Position: <span id=\"servoPos2\"></span></p>");
            client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider2\" oninput=\"updateServo(2, this.value)\">");

            // Motor control interface
            client.println("<p>Motor 1 Speed: <span id=\"motor1\"></span></p>");
            client.println("<input type=\"range\" min=\"-255\" max=\"255\" class=\"slider\" id=\"motor1Slider\" oninput=\"updateMotor(1, this.value)\">");
            client.println("<p>Motor 2 Speed: <span id=\"motor2\"></span></p>");
            client.println("<input type=\"range\" min=\"-255\" max=\"255\" class=\"slider\" id=\"motor2Slider\" oninput=\"updateMotor(2, this.value)\">");

            client.println("<script>");
            client.println("function updateServo(servo, value) {");
            client.println("  fetch(`/?servo${servo}=${value}`);");
            client.println("  document.getElementById(`servoPos${servo}`).textContent = value;");
            client.println("}");
            client.println("function updateMotor(motor, value) {");
            client.println("  fetch(`/?motor${motor}=${value}`);");
            client.println("  document.getElementById(`motor${motor}`).textContent = value;");
            client.println("}");
            client.println("</script></body></html>");

            // Parse request
            if (header.indexOf("GET /?servo1=") >= 0)
            {
              int index = header.indexOf("servo1=") + 7;
              valueString1 = header.substring(index);
              myservo1.write(valueString1.toInt());
            }
            if (header.indexOf("GET /?servo2=") >= 0)
            {
              int index = header.indexOf("servo2=") + 7;
              valueString2 = header.substring(index);
              myservo2.write(valueString2.toInt());
            }
            if (header.indexOf("GET /?motor1=") >= 0)
            {
              int index = header.indexOf("motor1=") + 7;
              motor1Speed = header.substring(index).toInt();
              setMotorSpeed(1, motor1Speed);
            }
            if (header.indexOf("GET /?motor2=") >= 0)
            {
              int index = header.indexOf("motor2=") + 7;
              motor2Speed = header.substring(index).toInt();
              setMotorSpeed(2, motor2Speed);
            }

            client.println();
            break;
          }
          else
          {
            currentLine = "";
          }
        }
        else if (c != '\r')
        {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
  }
}
