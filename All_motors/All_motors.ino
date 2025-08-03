// Motor 1 Pins
#define PUL_PIN1 5
#define DIR_PIN1 6
#define ENA_PIN1 7

// Motor 2 Pins
#define PUL_PIN2 11
#define DIR_PIN2 12
#define ENA_PIN2 13

// Motor 3 Pins
#define PUL_PIN3 2
#define DIR_PIN3 3
#define ENA_PIN3 4

const int stepsPerRevolution1 = 200;
const int stepsPerRevolution2 = 500; // Updated according to your driver settings
const int stepsPerRevolution3 = 3800;
  
void setup() {
  Serial.begin(9600);
  
  // Setup Motor 1
  pinMode(PUL_PIN1, OUTPUT);
  pinMode(DIR_PIN1, OUTPUT);
  pinMode(ENA_PIN1, OUTPUT);

  digitalWrite(ENA_PIN1, LOW);
  digitalWrite(DIR_PIN1, HIGH);

  // Setup Motor 2
  pinMode(PUL_PIN2, OUTPUT);
  pinMode(DIR_PIN2, OUTPUT);
  pinMode(ENA_PIN2, OUTPUT);

  digitalWrite(ENA_PIN2, LOW);

  // Setup Motor 3
  pinMode(PUL_PIN3, OUTPUT);
  pinMode(DIR_PIN3, OUTPUT);
  pinMode(ENA_PIN3, OUTPUT);

  digitalWrite(ENA_PIN3, LOW);
}

void loop() {
  // Run Motor 1
  Serial.println("Running Motor 1");
  for (int i = 0; i < 3 * stepsPerRevolution1; i++) {
    digitalWrite(PUL_PIN1, HIGH);
    delayMicroseconds(500);
    digitalWrite(PUL_PIN1, LOW);
    delayMicroseconds(500);
  }

  // Run Motor 2 clockwise
  Serial.println("Running Motor 2 Clockwise");
  digitalWrite(DIR_PIN2, HIGH);
  for (int i = 0; i < stepsPerRevolution2; i++) {
    digitalWrite(PUL_PIN2, HIGH);
    delayMicroseconds(2000);
    digitalWrite(PUL_PIN2, LOW);
    delayMicroseconds(2000);
  }

  // Turn Motor 2 back counterclockwise
  Serial.println("Returning Motor 2 Counterclockwise");
  digitalWrite(DIR_PIN2, LOW);
  for (int i = 0; i < stepsPerRevolution2; i++) {
    digitalWrite(PUL_PIN2, HIGH);
    delayMicroseconds(2000);
    digitalWrite(PUL_PIN2, LOW);
    delayMicroseconds(2000);
  }

  // Run Motor 3 clockwise first
  Serial.println("Running Motor 3 Clockwise");
  digitalWrite(DIR_PIN3, HIGH);
  for (int i = 0; i < 6 * stepsPerRevolution3; i++) {
    digitalWrite(PUL_PIN3, HIGH);
    delayMicroseconds(120);
    digitalWrite(PUL_PIN3, LOW);
    delayMicroseconds(120);
  }

  // Run Motor 3 back to initial position counterclockwise
  Serial.println("Returning Motor 3 Counterclockwise");
  digitalWrite(DIR_PIN3, LOW);
  for (int i = 0; i < 6 * stepsPerRevolution3; i++) {
    digitalWrite(PUL_PIN3, HIGH);
    delayMicroseconds(120);
    digitalWrite(PUL_PIN3, LOW);
    delayMicroseconds(120);
  }

  // Stop the code
  Serial.println("Process Complete");
  while(true);
}
