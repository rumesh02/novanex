#define PUL_PIN 8 // Pulse pin
#define DIR_PIN 9 // Direction pin
#define ENA_PIN 10 // Enable pin

const int stepsPerRevolution = 3800; // Adjust based on your stepper motor specifications

void setup() {
  pinMode(PUL_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENA_PIN, OUTPUT);

  digitalWrite(ENA_PIN, LOW); // Enable the motor driver
}

void loop() {
  turnMotor(6 * stepsPerRevolution, true); // Turn 6 rounds clockwise
  delay(2000); // Wait for a second
  turnMotor(6 * stepsPerRevolution, false); // Turn back 6 rounds counterclockwise
  delay(2000); // Wait for a second before next loop
}

void turnMotor(int steps, bool clockwise) {
  digitalWrite(DIR_PIN, clockwise ? HIGH : LOW); // Set direction
  
  for (int i = 0; i < steps; i++) {
    digitalWrite(PUL_PIN, HIGH);
    delayMicroseconds(120); // Adjust for speed
    digitalWrite(PUL_PIN, LOW);
    delayMicroseconds(120); // Adjust for speed
  }
}
