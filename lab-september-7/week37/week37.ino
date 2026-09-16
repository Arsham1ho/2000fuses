#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 37, en = 36, d4 = 35, d5 = 34, d6 = 33, d7 = 32;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int js_x = A8, js_y = A9, js_key = 19;

#define Motor_forward 0
#define Motor_return 1
#define Motor_L_dir_pin 7
#define Motor_R_dir_pin 8
#define Motor_L_pwm_pin 9
#define Motor_R_pwm_pin 10


// ISR STUFF FINISHED, BUT JANKY

volatile bool pushFlag = false;
volatile unsigned long lastStateChange = 0;
const unsigned long debounceMillis = 50;
volatile unsigned int pushCounter = 0;
volatile bool motorTestComplete = false;

void onJoystickPush() {
  pushFlag = true;
}


void setup() {
  Serial.begin(9600);
  pinMode(Motor_L_dir_pin, OUTPUT);
  pinMode(Motor_R_dir_pin, OUTPUT);
  pinMode(Motor_L_pwm_pin, OUTPUT);
  pinMode(Motor_R_pwm_pin, OUTPUT);
  pinMode(js_key, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(js_key), onJoystickPush, FALLING);
  lcd.begin(20, 4);
}


// SOMETIMES JOYSTICK VALUES DO NOT CHANGE INDEPENDENTLY, BOTH ARE THE SAME VALUE
// FAULT WITH CODE OR JOYSTICK? CLARIFY

void printJoystickValues() {
 int x_value = analogRead(js_x);
 int y_value = analogRead(js_y);
 double x_relative_volts = ( (double) x_value * 5.0) / 1023.0;
 double y_relative_volts = ( (double) y_value * 5.0) / 1023.0;
 int x_relative_percentage = (((x_relative_volts / 5) * 100) - 50) * 2;
 int y_relative_percentage = (((y_relative_volts / 5) * 100) - 50) * 2;
 
 lcd.setCursor(0, 0);
 lcd.print("x:");
 lcd.print(x_value);
 lcd.setCursor(14, 0);
 lcd.print(x_relative_percentage);
 lcd.print(" %");

lcd.setCursor(0, 1);
lcd.print("y:");
lcd.print(y_value);
lcd.setCursor(14, 1);
lcd.print(y_relative_percentage);
lcd.print(" %");
}

// SOMETHING IS NO LONGER FUCKY
// Motor test runs once in each direction, then the motorTestComplete flag is set to true
// This prevents any follow-up tests until the code is reuploaded
void motorTest() {
  digitalWrite(Motor_R_dir_pin, Motor_return);
  digitalWrite(Motor_L_dir_pin, Motor_return);

  // in the original code, both tires set to spin in the same direction, didn't happen in practice
  // test code altered so that both wheels spin in the same direction instead of opposite directions, not sure if intentional or not
  // CLARIFY

  for (int pwm = 255; pwm > 50; pwm--) {
    analogWrite(Motor_L_pwm_pin, pwm);
    analogWrite(Motor_R_pwm_pin, pwm);
    Serial.println(pwm);
    delay(50);
  }

  digitalWrite(Motor_R_dir_pin, Motor_forward);
  digitalWrite(Motor_L_dir_pin, Motor_forward);

  Serial.println();

  for (int pwm = 50; pwm < 255; pwm++) {
    analogWrite(Motor_L_pwm_pin, pwm);
    analogWrite(Motor_R_pwm_pin, pwm);
    Serial.println(pwm);
    delay(50);
  }
  analogWrite(Motor_L_pwm_pin, 0);
  analogWrite(Motor_R_pwm_pin, 0);
  motorTestComplete = true;
}

void printPushCounter() {
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Push counter: ");
   lcd.print(pushCounter);
   delay(1500);
   lcd.clear();
}

void loop() {
  if (pushFlag) {
    pushFlag = false;

    unsigned long now = millis();
    if (now - lastStateChange >= debounceMillis && !motorTestComplete) {
      digitalWrite(digitalPinToInterrupt(js_key), !digitalRead(digitalPinToInterrupt(js_key)));
      lastStateChange = now;
      //pushCounter++;
      //printPushCounter();
      motorTest();
    }
  }
  printJoystickValues();
  delay(200);
  lcd.clear();
}
