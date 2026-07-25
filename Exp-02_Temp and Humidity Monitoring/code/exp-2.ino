#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

#define DHTPIN 2
#define DHTTYPE DHT11

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);

  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("OLED initialization failed");
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
}

void loop() {

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,20);
    display.println("DHT11 Error!");
    display.display();

    Serial.println("Failed to read DHT11");
    delay(2000);
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" C   Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(10,0);
  display.println("DHT11");

  display.setTextSize(2);
  display.setCursor(0,25);
  display.print("T:");
  display.print(temperature,1);
  display.print((char)247);   // Degree symbol
  display.print("C");

  display.setCursor(0,48);
  display.print("H:");
  display.print(humidity,1);
  display.print("%");

  display.display();

  delay(2000);
}