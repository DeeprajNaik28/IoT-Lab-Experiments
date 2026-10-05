#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* apName = "ESP8266_Control";
const char* apPassword = "12345678";

ESP8266WebServer server(80);

// Built-in LED on most NodeMCU ESP8266 boards
#define LED_PIN 2

String webpage() {
  String html = "<!DOCTYPE html>";
  html += "<html>";
  html += "<head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>ESP8266 Control</title>";

  html += "<style>";
  html += "body {";
  html += "font-family: Arial;";
  html += "text-align: center;";
  html += "margin-top: 60px;";
  html += "}";
  html += "button {";
  html += "font-size: 25px;";
  html += "padding: 15px 35px;";
  html += "margin: 10px;";
  html += "}";
  html += "</style>";

  html += "</head>";
  html += "<body>";

  html += "<h1>ESP8266 LED Control</h1>";

  html += "<p>";
  html += "<a href='/on'><button>LED ON</button></a>";
  html += "</p>";

  html += "<p>";
  html += "<a href='/off'><button>LED OFF</button></a>";
  html += "</p>";

  html += "</body>";
  html += "</html>";

  return html;
}

void setup() {

  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);

  // LED OFF
  digitalWrite(LED_PIN, HIGH);

  // Create ESP8266 hotspot
  WiFi.softAP(apName, apPassword);

  Serial.println();
  Serial.println("ESP8266 Hotspot Started!");

  Serial.print("Wi-Fi Name: ");
  Serial.println(apName);

  Serial.print("Password: ");
  Serial.println(apPassword);

  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Main page
  server.on("/", []() {
    server.send(200, "text/html", webpage());
  });

  // LED ON
  server.on("/on", []() {
    digitalWrite(LED_PIN, LOW);
    server.send(200, "text/html", webpage());
  });

  // LED OFF
  server.on("/off", []() {
    digitalWrite(LED_PIN, HIGH);
    server.send(200, "text/html", webpage());
  });

  server.begin();

  Serial.println("Web server started!");
}

void loop() {
  server.handleClient();
}