#include <Wire.h>
#include <LiquidCrystal.h>

// PIN definitions

const int rs = 37, en = 36, d4 = 35, d5 = 34, d6 = 33, d7 = 32;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int js_x = A8, js_y = A9, js_key = 19;
const int SDA_pin = 20; // serial data
const int SCL_pin = 21; // serial clock


#define CMPS_address 0x60
#define Motor_forward 0
#define Motor_return 1
#define Motor_L_dir_pin 7
#define Motor_R_dir_pin 8
#define Motor_L_pwm_pin 9
#define Motor_R_pwm_pin 10

// Predefined global variables

int reading = 0;

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

  Wire.requestFrom(CMPS_address, 2);

  if (Wire.available() >= 2) {
    reading = map(Wire.read(), 0, 255, 0, 359);
  }

  if (reading <= 23) {
    lcd.setCursor(0, 1);
    lcd.print("Direction: ");
    lcd.print("N");

  } else if (reading >= 24 && reading < 68) {
    lcd.setCursor(0, 1);
    lcd.print("Direction: ");
    lcd.print("NE");

  } else if (reading >= 68 && reading < 113) {
    lcd.setCursor(0, 1);
    lcd.print("Direction: ");
    lcd.print("E");

  } else if (reading >= 113 && reading < 158) {
    lcd.setCursor(0, 1);
    lcd.print("Direction: ");
    lcd.print("SE");

  } else if (reading >= 158 && reading < 203) {
    lcd.setCursor(0, 1);
    lcd.print("Direction: ");
    lcd.print("S");

  } else if (reading >= 203 && reading < 248) {
    lcd.setCursor(0, 1);
    lcd.print("Direction: ");
    lcd.print("SW");


  } else if (reading >= 248 && reading < 293) {
    lcd.setCursor(0, 1);
    lcd.print("Direction: ");
    lcd.print("W");

  } else if (reading >= 293 && reading < 338) {
    lcd.setCursor(0, 1);
    lcd.print("Direction: ");
    lcd.print("NW");

  } else if (reading >= 338 && reading < 360) {
    lcd.setCursor(0, 1);
    lcd.print("Direction: ");
    lcd.print("N");

  }
  lcd.setCursor(0, 0);
  lcd.print("Reading: ");
  lcd.print(reading);
}

void turnLeft() {
  // left motor still goes the opposite way
  digitalWrite(Motor_R_dir_pin, Motor_forward);
  digitalWrite(Motor_L_dir_pin, Motor_forward);
  analogWrite(Motor_R_pwm_pin, 50);
  analogWrite(Motor_L_pwm_pin, 50);
}

void turnRight() {
  digitalWrite(Motor_R_dir_pin, Motor_return);
  digitalWrite(Motor_L_dir_pin, Motor_return);
  analogWrite(Motor_R_pwm_pin, 50);
  analogWrite(Motor_L_pwm_pin, 50);
}

void turnToHeading (unsigned int heading) {
  while (!(reading >= heading - 2) && (reading <= heading + 2)) {
    turnLeft();
  }
  analogWrite(Motor_R_pwm_pin, 0);
  analogWrite(Motor_L_pwm_pin, 0);
}

void loop() {
  printRawValue();
  printCompassHeading();
  turnToHeading(45);
  delay(25);
  lcd.clear();
}
