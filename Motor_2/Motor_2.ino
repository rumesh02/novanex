#define PUL_PIN 2 // Pulse pin
#define DIR_PIN 3 // Direction pin
#define ENA_PIN 4 // Enable pin

const int stepsPerRevolution = 6000; // Adjust based on your stepper motor specifications
const float stepsPerDegree = stepsPerRevolution / 360.0; // Steps per degree

void setup() {
  pinMode(PUL_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENA_PIN, OUTPUT);

  digitalWrite(ENA_PIN, LOW); // Enable the motor driver
}

void loop() {
  turnMotor(45, true); // Turn 45 degrees counterclockwise
  delay(1000); // Wait for a second
  turnMotor(45, false); // Turn back 45 degrees clockwise
  delay(1000); // Wait for a second before next loop
}

void turnMotor(int degrees, bool clockwise) {
  int steps = degrees * stepsPerDegree;
  digitalWrite(DIR_PIN, clockwise ? HIGH : LOW); // Set direction
  
  for (int i = 0; i < steps; i++) {
    digitalWrite(PUL_PIN, HIGH);
    delayMicroseconds(1000); // Adjust for speed
    digitalWrite(PUL_PIN, LOW);
    delayMicroseconds(1000); // Adjust for speed
  }
}
