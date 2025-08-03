#include <Wire.h>
#include <LiquidCrystal_I2C.h>
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

// Define relay pins
const int relay1Pin = 22;
const int relay2Pin = 23;

// Initialize the LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjust the I2C address if necessary

void setup() {
  Serial.begin(9600);
  
  // Initialize relay pins as output
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  
  // Turn off relays initially
  digitalWrite(relay1Pin, HIGH);
  digitalWrite(relay2Pin, HIGH);
  
  // Initialize the LCD
  lcd.begin(16,2);
  lcd.backlight();
  lcd.clear();
}

void loop() {
  // Read temperature from the first thermocouple
  double temperature1 = thermocouple1.readCelsius();
  Serial.print("Temperature 1: ");
  Serial.println(temperature1);
  
  // Read temperature from the second thermocouple
  double temperature2 = thermocouple2.readCelsius();
  Serial.print("Temperature 2: ");
  Serial.println(temperature2);
  
  // Display temperatures on the LCD
  lcd.setCursor(0, 0);
  lcd.print("Temp1: ");
  lcd.print(temperature1);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Temp2: ");
  lcd.print(temperature2);
  lcd.print(" C");
  
  // Control the first relay based on the first temperature sensor
  if (temperature1 > 50.0) {
    digitalWrite(relay1Pin, HIGH);
  } else {
    digitalWrite(relay1Pin, LOW);
  }
  
  // Control the second relay based on the second temperature sensor
  if (temperature2 > 50.0) {
    digitalWrite(relay2Pin, HIGH);
  } else {
    digitalWrite(relay2Pin, LOW);
  }
  
  delay(1000); // Wait for a second before the next reading
}
