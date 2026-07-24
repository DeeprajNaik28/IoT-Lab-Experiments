const int flamePin = 2;     // Flame sensor DO
const int gasPin = A0;      // MQ2 AO

const int fireLED = 8;
const int gasLED = 9;
const int normalLED = 10;

int gasThreshold = 300;     // Adjust after testing

void setup() {
  pinMode(flamePin, INPUT);

  pinMode(fireLED, OUTPUT);
  pinMode(gasLED, OUTPUT);
  pinMode(normalLED, OUTPUT);

  Serial.begin(9600);
}

void loop() {

  int flame = digitalRead(flamePin);
  int gasValue = analogRead(gasPin);

  Serial.print("Gas Value: ");
  Serial.print(gasValue);
  Serial.print("  Flame: ");
  Serial.println(flame);

  // Flame sensor outputs LOW when fire is detected
  if (flame == LOW) {
    digitalWrite(fireLED, HIGH);
    digitalWrite(gasLED, LOW);
    digitalWrite(normalLED, LOW);
  }
  else if (gasValue > gasThreshold) {
    digitalWrite(fireLED, LOW);
    digitalWrite(gasLED, HIGH);
    digitalWrite(normalLED, LOW);
  }
  else {
    digitalWrite(fireLED, LOW);
    digitalWrite(gasLED, LOW);
    digitalWrite(normalLED, HIGH);
  }

  delay(200);
}