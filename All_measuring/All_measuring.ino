#include <HX711.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>

#define LOADCELL_DOUT_PIN 6
#define LOADCELL_SCK_PIN 7
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

float targetWeight = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting setup...");
  setupLoadCell();
  setupLCD();
  setupServo();
  closeLid(); // Ensure the lid is closed at the beginning
  lcd.clear();
  displayWeight(getWeight());
}

void loop() {
  displayWeight(getWeight());
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

    float currentWeight = getWeight();
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
      float currentWeight = getWeight();
      Serial.print("Current weight: ");
      Serial.println(currentWeight);
      
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
  } else if (key == 'B') {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Taring...");
    scale.tare();
    lcd.clear();
    displayWeight(getWeight());
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

void openLid() {
  lidServo.write(155);  // Adjust angle as needed to open the lid
}

void closeLid() {
  lidServo.write(0);  // Adjust angle as needed to close the lid
}
