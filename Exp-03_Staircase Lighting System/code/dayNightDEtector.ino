const int ldrPin = A0;
const int pirPin = 2;
const int ledPin = 13;

// Adjust this after testing your LDR
const int threshold = 500;

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int ldrValue = analogRead(ldrPin);
  int motion = digitalRead(pirPin);

  Serial.print("LDR: ");
  Serial.print(ldrValue);
  Serial.print("  Motion: ");
  Serial.println(motion);

  // Day
  if (ldrValue > threshold) {
      if (motion == HIGH) {
      digitalWrite(ledPin, HIGH);
      delay(2000);
    } else {
      digitalWrite(ledPin, LOW);
    }
  }
  // Night
  else {
      digitalWrite(ledPin, LOW);

  }

  delay(100);
}