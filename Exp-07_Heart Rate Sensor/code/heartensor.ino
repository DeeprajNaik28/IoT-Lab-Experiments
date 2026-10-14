#define USE_ARDUINO_INTERRUPTS true

#include <PulseSensorPlayground.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ================= OLED =================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ================= HEART SENSOR =================

#define PULSE_PIN A0
#define THRESHOLD 520

PulseSensorPlayground pulseSensor;

// ================= BUZZER =================

#define BUZZER_PIN 5

// ================= BUTTON =================

#define BUTTON_PIN 2

// ================= LIMITS =================

#define HIGH_BPM 100

#define MIN_VALID_BPM 40
#define MAX_VALID_BPM 180

int BPM = 0;
int lastValidBPM = 0;

unsigned long lastBeatTime = 0;

// Buzzer state
bool buzzerMuted = false;

// Button state
bool lastButtonState = HIGH;

void setup() {

  Serial.begin(9600);

  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);

  // Button
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // ================= OLED =================

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {

    Serial.println("OLED not found!");

    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(20, 5);
  display.println("HEART");

  display.setCursor(30, 30);
  display.println("RATE");

  display.display();

  delay(2000);

  // ================= PULSE SENSOR =================

  pulseSensor.analogInput(PULSE_PIN);
  pulseSensor.setThreshold(THRESHOLD);

  pulseSensor.begin();

  // ================= READY SCREEN =================

  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(25, 20);
  display.println("Place finger");

  display.setCursor(30, 35);
  display.println("on sensor");

  display.display();

  delay(2000);
}

void loop() {

  // ==================================================
  // BUTTON CHECK
  // ==================================================

  bool buttonState = digitalRead(BUTTON_PIN);

  // Button pressed
  if (lastButtonState == HIGH && buttonState == LOW) {

    // Silence buzzer
    noTone(BUZZER_PIN);

    buzzerMuted = true;

    Serial.println("BUZZER MUTED");

    delay(200);
  }

  lastButtonState = buttonState;

  // ==================================================
  // HEARTBEAT DETECTION
  // ==================================================

  if (pulseSensor.sawStartOfBeat()) {

    BPM = pulseSensor.getBeatsPerMinute();

    // Accept only realistic readings
    if (BPM >= MIN_VALID_BPM && BPM <= MAX_VALID_BPM) {

      lastValidBPM = BPM;
      lastBeatTime = millis();

      // ==================================================
      // SEND BPM TO PYTHON
      // ==================================================

      Serial.println(lastValidBPM);

      // ==================================================
      // HIGH HEART RATE
      // ==================================================

      if (lastValidBPM > HIGH_BPM) {

        // Only sound if not muted
        if (!buzzerMuted) {

          tone(BUZZER_PIN, 2000);
        }

      } else {

        // Normal heart rate
        noTone(BUZZER_PIN);

        // Reset mute when heart rate becomes normal
        buzzerMuted = false;
      }

      // ==================================================
      // OLED
      // ==================================================

      display.clearDisplay();

      display.setTextColor(SSD1306_WHITE);

      // Title
      display.setTextSize(1);
      display.setCursor(35, 0);
      display.println("HEART RATE");

      // Heart
      display.setTextSize(2);
      display.setCursor(5, 20);
      display.print("<3");

      // BPM
      display.setCursor(45, 20);
      display.print(lastValidBPM);

      display.setTextSize(1);
      display.setCursor(95, 27);
      display.println("BPM");

      // Status
      if (lastValidBPM > HIGH_BPM) {

        display.setCursor(10, 50);

        if (buzzerMuted) {
          display.println("HIGH - MUTED");
        } else {
          display.println("HIGH HEART RATE!");
        }

      } else {

        display.setCursor(40, 50);
        display.println("NORMAL");
      }

      display.display();
    }
  }

  // ==================================================
  // NO FINGER DETECTED
  // ==================================================

  if (millis() - lastBeatTime > 5000) {

    noTone(BUZZER_PIN);

    buzzerMuted = false;

    display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(1);

    display.setCursor(35, 5);
    display.println("HEART RATE");

    display.setCursor(20, 25);
    display.println("Place finger");

    display.setCursor(30, 40);
    display.println("on sensor");

    display.display();
  }

  delay(20);
}