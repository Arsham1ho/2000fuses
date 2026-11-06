#include <Wire.h>
#include <LiquidCrystal.h>

// PIN definitions

const int rs = 37, en = 36, d4 = 35, d5 = 34, d6 = 33, d7 = 32;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int js_x = A8, js_y = A9;
int buttonPin = 19;
int buttonState = 1;
const int SDA_pin = 20; // serial data
const int SCL_pin = 21; // serial clock

const int encL_pin = 2;
const int encR_pin = 3;

#define CMPS_address 0x60
#define Motor_forward 0
#define Motor_return 1
#define Motor_L_dir_pin 7
#define Motor_R_dir_pin 8
#define Motor_L_pwm_pin 9
#define Motor_R_pwm_pin 10

const int TOLERANCE = 4;
const int PWM_25 = 64;
const int PWM_50 = 128;
const int PWM_75 = 191;
const float PULSES_PER_CM = 10.0;   // CALIBRATE THIS
const bool RUN_AT_START = true;

// Predefined global variables
volatile bool pushFlag = false;
volatile long pulsesL = 0;
volatile long pulsesR = 0;
int reading = 0;
bool compassOk = false;

void isrL() { pulsesL++; }
void isrR() { pulsesR++; }

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(Motor_L_dir_pin, OUTPUT);
  pinMode(Motor_R_dir_pin, OUTPUT);
  pinMode(Motor_L_pwm_pin, OUTPUT);
  pinMode(Motor_R_pwm_pin, OUTPUT);
  pinMode(encL_pin, INPUT_PULLUP);
  pinMode(encR_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encL_pin), isrL, RISING);
  attachInterrupt(digitalPinToInterrupt(encR_pin), isrR, RISING);
  lcd.begin(20, 4);

  if (RUN_AT_START) {
    delay(1000);
    Exrecise5();
  }
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

  Wire.requestFrom(CMPS_address, 1);

  compassOk = false;
  if (Wire.available() >= 1) {
    reading = map(Wire.read(), 0, 255, 0, 359);
    compassOk = true;
  }

  if (reading < 23) {
    lcd.setCursor(0, 1);
    lcd.print("Direction: ");
    lcd.print("N ");

  } else if (reading >= 23 && reading < 68) {
    lcd.setCursor(0, 1);
    lcd.print("Direction: ");
    lcd.print("NE");

  } else if (reading >= 68 && reading < 113) {
    lcd.setCursor(0, 1);
    lcd.print("Direction: ");
    lcd.print("E ");

  } else if (reading >= 113 && reading < 158) {
    lcd.setCursor(0, 1);
    lcd.print("Direction: ");
    lcd.print("SE");

  } else if (reading >= 158 && reading < 203) {
    lcd.setCursor(0, 1);
    lcd.print("Direction: ");
    lcd.print("S ");

  } else if (reading >= 203 && reading < 248) {
    lcd.setCursor(0, 1);
    lcd.print("Direction: ");
    lcd.print("SW");

  } else if (reading >= 248 && reading < 293) {
    lcd.setCursor(0, 1);
    lcd.print("Direction: ");
    lcd.print("W ");

  } else if (reading >= 293 && reading < 338) {
    lcd.setCursor(0, 1);
    lcd.print("Direction: ");
    lcd.print("NW");

  } else {
    lcd.setCursor(0, 1);
    lcd.print("Direction: ");
    lcd.print("N ");
  }
  lcd.setCursor(0, 0);
  lcd.print("Reading: ");
  lcd.print(reading);
  lcd.print("   ");
}

void turnLeft() {
  // left motor still goes the opposite way
  digitalWrite(Motor_R_dir_pin, Motor_forward);
  digitalWrite(Motor_L_dir_pin, Motor_forward);
  analogWrite(Motor_R_pwm_pin, 80);
  analogWrite(Motor_L_pwm_pin, 80);
}

void turnRight() {
  digitalWrite(Motor_R_dir_pin, Motor_return);
  digitalWrite(Motor_L_dir_pin, Motor_return);
  analogWrite(Motor_R_pwm_pin, 80);
  analogWrite(Motor_L_pwm_pin, 80);
}

void stopMotors() {
  analogWrite(Motor_R_pwm_pin, 0);
  analogWrite(Motor_L_pwm_pin, 0);
}

int headingError(int heading) {
  return ((heading - reading + 540) % 360) - 180;
}
void turnToHeading(unsigned int heading) {
  printCompassHeading();
  while (compassOk && abs(headingError((int)heading)) > TOLERANCE) {
    if (headingError((int)heading) > 0) {
      turnRight();
    } else {
      turnLeft();
    }
    printCompassHeading();
  }
  stopMotors();
  delay(300);
}

void driveCm(float cm, int pwm) {
  long target = (long)(cm * PULSES_PER_CM);

  noInterrupts();
  pulsesL = 0;
  pulsesR = 0;
  interrupts();

  digitalWrite(Motor_R_dir_pin, Motor_forward);
  digitalWrite(Motor_L_dir_pin, Motor_return);
  analogWrite(Motor_R_pwm_pin, pwm);
  analogWrite(Motor_L_pwm_pin, pwm);

  unsigned long startTime = millis();
  while (millis() - startTime < 10000) {
    noInterrupts();
    long l = pulsesL;
    long r = pulsesR;
    interrupts();

    if ((l + r) / 2 >= target) break;
  }

  stopMotors();
  delay(300);
}

void Exrecise5() {
  printCompassHeading();
  if (!compassOk) return;

  int startHeading = reading;

  driveCm(13, PWM_25);
  turnToHeading((startHeading - 110 + 360) % 360);

  driveCm(20, PWM_50);
  turnToHeading((startHeading - 250 + 360) % 360);

  driveCm(20, PWM_75);
  turnToHeading(startHeading);

  delay(30000000);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  Serial.print(buttonPin);

  if (buttonState == 0) {
    Serial.println("Works");
    Exrecise5();
    while (digitalRead(buttonPin) == 1) {}
    delay(50);

  }

  printRawValue();
  printCompassHeading();
  delay(25);
}
