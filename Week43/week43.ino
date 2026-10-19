#include <Wire.h>
#include <LiquidCrystal.h>

//PIN definitions

const int rs = 37, en = 36, d4 = 35, d5 = 34, d6 = 33, d7 = 32;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int js_x = A8, js_y = A9, js_key = 19;
const int SDA_pin = 20; // serial data
const int SCL_pin = 21; // serial clock

#define CMPS_address 0x60


void setup() {
  Serial.begin(9600);
  Wire.begin();
  lcd.begin(20, 4);
}

void printRawValue() {
  Wire.beginTransmission(CMPS_address);
  Wire.write(byte(0x01));
  Wire.endTransmission(false);

  Wire.requestFrom(CMPS_address, 1, false);

  if (Wire.available() >= 1) {
    byte raw = Wire.read();
    
    Serial.print("raw: ");
    Serial.print(raw);
    Serial.println();
  }
}


void printCompassHeading() {
  Wire.beginTransmission(CMPS_address);
  Wire.write(byte(0x01));
  Wire.endTransmission();
  long reading;

  Wire.requestFrom(CMPS_address, 2);

  if (Wire.available() >= 2) {
    reading = map(Wire.read(), 0, 255, 0, 359);
  }

  lcd.setCursor(0, 0);
  lcd.print("Reading: ");
  lcd.print(reading);
 }

void loop() {
  printRawValue();
  printCompassHeading();
  delay(25);
  lcd.clear();
}
