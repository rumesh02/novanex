#include <HX711.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>
#include <max6675.h>

// Load Cell Pins
#define LOADCELL_DOUT_PIN 32
#define LOADCELL_SCK_PIN 33
HX711 scale;
float calibration_factor = 191;

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Keypad
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {22, 23, 24, 25};
byte colPins[COLS] = {26, 27, 28, 29};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Servo
Servo lidServo;
#define SERVO_PIN 9

// LEDs
#define LED1_PIN 30  // LED1 for weight above 1000g
#define LED2_PIN 31  // LED2 for weight below 1000g

// Temperature Sensor Pins

int thermoDO1 = 50;
int thermoCS1 = 53;
int thermoCLK1 = 52;
int thermoDO2 = 48;
int thermoCS2 = 49;
int thermoCLK2 = 52; // Sharing the clock pin
MAX6675 thermocouple1(thermoCLK1, thermoCS1, thermoDO1);
MAX6675 thermocouple2(thermoCLK2, thermoCS2, thermoDO2);

// Relay Pins
const int relay1Pin = 44;
const int relay2Pin = 45;

// Stepper Motor Pins
#define PUL_PIN1 5
#define DIR_PIN1 6
#define ENA_PIN1 7
#define PUL_PIN2 11
#define DIR_PIN2 12
#define ENA_PIN2 13
#define PUL_PIN3 2
#define DIR_PIN3 3
#define ENA_PIN3 4

const int stepsPerRevolution1 = 200;
const int stepsPerRevolution2 = 500; // Updated according to your driver settings
const int stepsPerRevolution3 = 4200;

float targetWeight = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting setup...");
  
  setupLoadCell();
  setupLCD();
  setupServo();
  setupLEDs();
  setupMotors();
  setupTemperatureSensors();
  setupRelays();

  closeLid(); // Ensure the lid is closed at the beginning
  lcd.clear();
  displayWeight(getWeight());
}

void loop() {
  float currentWeight = getWeight();
  displayWeight(currentWeight);
  updateLEDs(currentWeight);
  updateTemperatureControl();

  char key = getKeypadInput();

  if (key == 'A') {
    handleGrainDispensing(currentWeight);
  } else if (key == 'B') {
    tareScale();
  }
}

void setupLoadCell() {
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
}

void setupLCD() {
  lcd.init();
  lcd.backlight();
}

void setupServo() {
  lidServo.attach(SERVO_PIN);
}

void setupLEDs() {
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
}

void setupMotors() {
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

void setupTemperatureSensors() {
  // Already done in object initialization
}

void setupRelays() {
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  digitalWrite(relay1Pin, HIGH);
  digitalWrite(relay2Pin, HIGH);
}

float getWeight() {
  return scale.get_units(10);
}

void displayWeight(float weight) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Weight: ");
  lcd.print(weight, 1);
  lcd.print(" g");
}

char getKeypadInput() {
  return keypad.getKey();
}

void updateLEDs(float weight) {
  if (weight > 2000) {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, LOW);
  } else {
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, HIGH);
  }
}

void openLid() {
  lidServo.write(155);  // Adjust angle as needed to open the lid
}

void closeLid() {
  lidServo.write(0);  // Adjust angle as needed to close the lid
}

void handleGrainDispensing(float currentWeight) {
  lcd.clear();
  lcd.print("Enter grams:");
  targetWeight = 0;
  String input = "";

  while (true) {
    char key = getKeypadInput();
    if (key) {
      if (key >= '0' && key <= '9') {
        input += key;
        lcd.setCursor(0, 1);
        lcd.print(input);
      } else if (key == '*') {
        if (input.length() > 0) {
          input.remove(input.length() - 1);  // Remove the last character
          lcd.setCursor(0, 1);
          lcd.print("                "); // Clear the line
          lcd.setCursor(0, 1);
          lcd.print(input);  // Display the updated input    
        }
      } else if (key == '#') {
        if (input.length() > 0) {
          targetWeight = input.toFloat();
          break;
        } else {
          lcd.clear();
          lcd.print("Enter a fair amount");
          delay(2000);  // Display warning for 2 seconds
          lcd.clear();
          lcd.print("Enter grams:");
          input = "";  // Clear the input string
        }
      }
    }
  }

  float finalWeight = currentWeight - targetWeight + 15;

  lcd.clear();
  lcd.print("Dispensing...");
  Serial.print("Current weight: ");
  Serial.println(currentWeight);
  Serial.print("Target weight to dispense: ");
  Serial.println(targetWeight);
  Serial.print("Final weight: ");
  Serial.println(finalWeight);

  openLid();

  // Wait until the current weight is close to the final weight
  while (getWeight() > finalWeight) {
    currentWeight = getWeight();
    Serial.print("Current weight: ");
    Serial.println(currentWeight);
    updateLEDs(currentWeight);
    
    // Close the lid incrementally as we approach the target weight
    if (currentWeight <= currentWeight - (targetWeight * 0.8)) {
      lidServo.write(120);  // Close the lid partially
    }
    if (currentWeight <= currentWeight - (targetWeight * 0.9)) {
      lidServo.write(90);  // Close the lid further
    }
    if (currentWeight <= finalWeight) {
      lidServo.write(0);  // Close the lid completely
      break;
    }
    delay(100);  // Short delay to allow weight measurement to update
  }

  closeLid();

  lcd.clear();
  lcd.print("Done");
  delay(2000);

  displayWeight(getWeight());

  // Start the sealing process after dispensing
  runSealingProcess();
}

void tareScale() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Taring...");
  scale.tare();
  lcd.clear();
  displayWeight(getWeight());
}

void runSealingProcess() {
  // Run Motor 1
  Serial.println("Running Motor 1");
  for (int i = 0; i < 7 * stepsPerRevolution1; i++) {
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

  // Run Motor 3 clockwise
  Serial.println("Running Motor 3 Clockwise");
  digitalWrite(DIR_PIN3, HIGH);
  for (int i = 0; i < 5 * stepsPerRevolution3; i++) {
    digitalWrite(PUL_PIN3, HIGH);
    delayMicroseconds(100);
    digitalWrite(PUL_PIN3, LOW);
    delayMicroseconds(100);
  }

  // Turn Motor 3 back counterclockwise
  Serial.println("Returning Motor 3 Counterclockwise");
  digitalWrite(DIR_PIN3, LOW);
  for (int i = 0; i < 5 * stepsPerRevolution3; i++) {
    digitalWrite(PUL_PIN3, HIGH);
    delayMicroseconds(100);
    digitalWrite(PUL_PIN3, LOW);
    delayMicroseconds(100);
  }

  Serial.println("Sealing Process Complete");
}

void updateTemperatureControl() {
  double temp1 = thermocouple1.readCelsius();
  double temp2 = thermocouple2.readCelsius();

  // Control the first relay based on the first temperature sensor
  if (temp1 > 50.0) {
    digitalWrite(relay1Pin, HIGH);
  } else {
    digitalWrite(relay1Pin, LOW);
  }
  
  // Control the second relay based on the second temperature sensor
  if (temp2 > 50.0) {
    digitalWrite(relay2Pin, HIGH);
  } else {
    digitalWrite(relay2Pin, LOW);
  }
}

