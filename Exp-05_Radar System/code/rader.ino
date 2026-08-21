#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Servo radarServo;

#define TRIG_PIN 9
#define ECHO_PIN 10
#define SERVO_PIN 3

#define MAX_DISTANCE 30      // Radar range (cm)
#define DETECT_DISTANCE 10   // Detection limit

// Radar center
const int cx = 64;
const int cy = 63;

float getDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if(duration == 0)
    return MAX_DISTANCE + 5;

  return duration * 0.0343 / 2.0;
}

void drawRadar()
{
  display.drawCircle(cx, cy, 20, SSD1306_WHITE);
  display.drawCircle(cx, cy, 40, SSD1306_WHITE);
  display.drawCircle(cx, cy, 60, SSD1306_WHITE);

  display.drawLine(4, cy, 124, cy, SSD1306_WHITE);
}

void drawSweep(int angle)
{
  float rad = radians(angle);

  int x = cx + 60 * cos(rad);
  int y = cy - 60 * sin(rad);

  display.drawLine(cx, cy, x, y, SSD1306_WHITE);
}

void drawTarget(int angle, float distance)
{
  if(distance > DETECT_DISTANCE)
    return;

  int r = map(distance * 10, 0, MAX_DISTANCE * 10, 0, 60);

  float rad = radians(angle);

  int x = cx + r * cos(rad);
  int y = cy - r * sin(rad);

  display.fillCircle(x, y, 2, SSD1306_WHITE);
}

void drawInfo(int angle, float distance)
{
  display.fillRect(0, 0, 128, 10, SSD1306_BLACK);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0,0);
  display.print(angle);
  display.print((char)247);

  display.setCursor(40,0);
  display.print(distance,1);
  display.print("cm");

  display.setCursor(90,0);

  if(distance <= DETECT_DISTANCE)
    display.print("OBJ");
  else
    display.print("---");
}

void scan(int startAngle, int endAngle, int step)
{
  for(int angle=startAngle;
      (step>0)? angle<=endAngle : angle>=endAngle;
      angle+=step)
  {

    radarServo.write(angle);
    delay(2);

    float distance = getDistance();

    display.clearDisplay();

    drawRadar();
    drawSweep(angle);
    drawTarget(angle, distance);
    drawInfo(angle, distance);

    display.display();

    Serial.print(angle);
    Serial.print(",");
    Serial.println(distance);
  }
}

void setup()
{
  Serial.begin(9600);

  radarServo.attach(SERVO_PIN);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    while(1);

  display.clearDisplay();
  display.display();
}

void loop()
{
  scan(0,180,2);
  scan(180,0,-2);
}