constexpr int kLedPin = LED_BUILTIN;

void setup() {
  pinMode(kLedPin, OUTPUT);
}

void loop() {
  digitalWrite(kLedPin, HIGH);
  delay(500);
  digitalWrite(kLedPin, LOW);
  delay(500);
}
