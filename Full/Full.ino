#include <HX711.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>
#include <max6675.h>

// Define the pins for the first MAX6675
int thermoDO1 = 50;
int thermoCS1 = 53;
int thermoCLK1 = 52;

// Define the pins for the second MAX6675
int thermoDO2 = 48;
int thermoCS2 = 49;
int thermoCLK2 = 52; // Sharing the clock pin

MAX6675 thermocouple1(thermoCLK1, thermoCS1, thermoDO1);
MAX6675 thermocouple2(thermoCLK2, thermoCS2, thermoDO2);

#define LOADCELL_DOUT_PIN 32
#define LOADCELL_SCK_PIN 33
HX711 scale;
float calibration_factor = 191;

LiquidCrystal_I2C lcd(0x27, 16, 2);

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

Servo lidServo;
#define SERVO_PIN 9

#define LED1_PIN 30  // LED1 for weight above 1000g
#define LED2_PIN 31  // LED2 for weight below 1000g

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

float targetWeight = 0;
bool motorOperationsExecuted = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting setup...");
  setupLoadCell();
  setupLCD();
  setupServo();
  setupLEDs();
  setupMotors();
  closeLid(); // Ensure the lid is closed at the beginning
  lcd.clear();
  displayWeight(getWeight());
}

void loop() {
  float currentWeight = getWeight();
  displayWeight(currentWeight);
  updateLEDs(currentWeight);

  char key = getKeypadInput();

  if (key == 'A') {
    lcd.clear();
    lcd.print("Enter grams:");
    targetWeight = 0;
    String input = "";

    while (true) {
      key = getKeypadInput();
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

    float finalWeight = currentWeight - targetWeight;

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
        delay(5000);
        motorOperationsExecuted = true;
        break;
      }
      delay(100);  // Short delay to allow weight measurement to update
    }

    closeLid();

    lcd.clear();
    lcd.print("Done");
    delay(2000);

    displayWeight(getWeight());
  } else if (key == 'B') {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Taring...");
    scale.tare();
    lcd.clear();
    displayWeight(getWeight());

    // Display temperatures on the LCD when 'B' is pressed
    displayTemperatures();
  }

  // Check if the motors need to be run after 5 seconds from lid close
  if (motorOperationsExecuted) {
    runMotorOperations();
    motorOperationsExecuted = false;
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

void runMotorOperations() {
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
}

void displayTemperatures() {


  
  double temperature1 = thermocouple1.readCelsius();
  double temperature2 = thermocouple2.readCelsius();
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp1: ");
  lcd.print(temperature1);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Temp2: ");
  lcd.print(temperature2);
  lcd.print(" C");

  delay(3000); // Display temperatures for 3 seconds before clearing
  lcd.clear();
}
