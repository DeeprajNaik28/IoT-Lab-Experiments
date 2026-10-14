
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>

// Same Wi-Fi credentials
const char* ssid = "Batman ";
const char* password = "123456789";

// Same ThingSpeak Write API Key
const char* writeAPIKey = "Q2IRYIIOKQ1NX02S";

// DHT11 sensor connected to D2
#define DHTPIN D2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;

// Timing settings
const unsigned long SENSOR_INTERVAL = 2000;
const unsigned long UPLOAD_INTERVAL = 20000;

unsigned long lastSensorRead = 0;
unsigned long lastUpload = 0;

float temperature = NAN;
float humidity = NAN;

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  unsigned long startAttempt = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - startAttempt < 20000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWi-Fi connection failed. Will retry.");
  }
}

void uploadToThingSpeak() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Upload skipped: Wi-Fi disconnected.");
    return;
  }

  HTTPClient http;

  String url = "http://api.thingspeak.com/update?api_key=";
  url += writeAPIKey;
  url += "&field1=" + String(temperature, 1);
  url += "&field2=" + String(humidity, 1);

  http.setTimeout(8000);

  if (!http.begin(client, url)) {
    Serial.println("Could not start HTTP request.");
    return;
  }

  int responseCode = http.GET();

  if (responseCode == 200) {
    String response = http.getString();
    response.trim();

    long entryID = response.toInt();

    if (entryID > 0) {
      Serial.print("ThingSpeak upload successful! Entry ID: ");
      Serial.println(entryID);
    } else {
      Serial.println(
        "Upload rejected. Check API key and channel update interval."
      );
    }
  } else {
    Serial.print("HTTP error: ");
    Serial.println(responseCode);
  }

  http.end();
}

void setup() {
  Serial.begin(115200);

  dht.begin();

  Serial.println();
  Serial.println("==============================");
  Serial.println("ESP8266 DHT11 Monitor");
  Serial.println("Temperature and Humidity");
  Serial.println("==============================");

  WiFi.mode(WIFI_STA);
  connectWiFi();
}

void loop() {
  unsigned long now = millis();

  // Reconnect if Wi-Fi disconnects
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  // Read DHT11 every 2 seconds
  if (now - lastSensorRead >= SENSOR_INTERVAL) {
    lastSensorRead = now;

    float newTemperature = dht.readTemperature();
    float newHumidity = dht.readHumidity();

    if (isnan(newTemperature) || isnan(newHumidity)) {
      Serial.println("DHT11 reading failed. Check wiring.");
    } else {
      temperature = newTemperature;
      humidity = newHumidity;

      Serial.println("------------------------------");

      Serial.print("Temperature: ");
      Serial.print(temperature, 1);
      Serial.println(" °C");

      Serial.print("Humidity: ");
      Serial.print(humidity, 1);
      Serial.println(" %");
    }
  }

  // Upload every 20 seconds
  if (now - lastUpload >= UPLOAD_INTERVAL) {
    lastUpload = now;

    if (!isnan(temperature) && !isnan(humidity)) {
      uploadToThingSpeak();
    } else {
      Serial.println("Upload skipped: no valid DHT11 reading.");
    }
  }

  delay(10);
}
