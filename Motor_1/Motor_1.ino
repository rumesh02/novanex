#define PUL_PIN 5 // Pulse pin
#define DIR_PIN 6 // Direction pin
#define ENA_PIN 7 // Enable pin

const int stepsPerRevolution = 200; // Adjust based on your stepper motor specifications

void setup() {
  pinMode(PUL_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENA_PIN, OUTPUT);

  digitalWrite(ENA_PIN, LOW); // Enable the motor driver
  digitalWrite(DIR_PIN, HIGH); // Set direction to counterclockwise
}

void loop() {
  for (int i = 0; i < 3 * stepsPerRevolution; i++) { // 3 rounds counterclockwise
    digitalWrite(PUL_PIN, HIGH);
    delayMicroseconds(500); // Adjust for speed
    digitalWrite(PUL_PIN, LOW);
    delayMicroseconds(500); // Adjust for speed
  }
  delay(1000); // Wait for a second before next loop
}
