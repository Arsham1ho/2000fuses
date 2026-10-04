#include <LiquidCrystal.h>

const int rs = 37, en = 36, d4 = 35, d5 = 34, d6 = 33, d7 = 32;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//temporarily reversed x and y pins for race-day ergonomics, sep. 26
const int js_x = A9, js_y = A8, js_key = 19;

#define Motor_forward 0
#define Motor_return 1
#define Motor_L_dir_pin 7
#define Motor_R_dir_pin 8
#define Motor_L_pwm_pin 9
#define Motor_R_pwm_pin 10

volatile bool pushFlag = false;
volatile unsigned long lastStateChange = 0;
const unsigned long debounceMillis = 50;
volatile unsigned int pushCounter = 0;
volatile bool motorTestComplete = false;
volatile bool simpleTurnsComplete = false;

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
 lcd.clear();
 int x_value = analogRead(js_x);
 int y_value = analogRead(js_y);
 double x_relative_volts = ( (double) x_value * 5.0) / 1023.0;
 double y_relative_volts = ( (double) y_value * 5.0) / 1023.0;
 int x_relative_percentage = (((x_relative_volts / 5) * 100) - 50) * 2;
 int y_relative_percentage = (((y_relative_volts / 5) * 100) - 50) * 2;

  if (x_relative_percentage < 0) {
  x_relative_percentage = x_relative_percentage * -1;
 }

 if (y_relative_percentage < 0) {
  y_relative_percentage = y_relative_percentage * -1;
 }
 
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

delay(100);
}

void simpleTurns() {
  digitalWrite(Motor_L_dir_pin, Motor_forward);
  digitalWrite(Motor_R_dir_pin, Motor_return);

  int pwm = 255 * 0.3;

  analogWrite(Motor_L_pwm_pin, pwm);
  delay(2000);
  analogWrite(Motor_L_pwm_pin, 0);
  analogWrite(Motor_R_pwm_pin, pwm);
  delay(1000);
  analogWrite(Motor_R_pwm_pin, 0);

  pwm = 255 * 0.75;
  
  digitalWrite(Motor_L_dir_pin, Motor_return);
  digitalWrite(Motor_R_dir_pin, Motor_forward);
  analogWrite(Motor_L_pwm_pin, pwm);
  analogWrite(Motor_R_pwm_pin, pwm);
  delay(4000);

  pwm = 0;

  analogWrite(Motor_L_pwm_pin, pwm);
  analogWrite(Motor_R_pwm_pin, pwm);
  simpleTurnsComplete = true;
}

void moveWithJoystick() {
  // x negative = left
  // x positive = right
  // y negative = back
  // y positive = forward

 int x_value = analogRead(js_x);
 int y_value = analogRead(js_y);
 double x_relative_volts = ( ((double) x_value + 1) * 5.0 ) / 1023.0;
 double y_relative_volts = ( ((double) y_value - 8) * 5.0 ) / 1023.0;
 int x_relative_percentage = abs( (((x_relative_volts / 5) * 100) - 50) * 2 );
 int y_relative_percentage = abs( (((y_relative_volts / 5) * 100) - 50) * 2 );

 int x_rel_pwm = 255 / 100 * x_relative_percentage;
 int y_rel_pwm = 255 / 100 * y_relative_percentage;

 if (x_relative_percentage == 0 && y_relative_percentage == 0) {
  analogWrite(Motor_L_pwm_pin, 0);
  analogWrite(Motor_R_pwm_pin, 0);
 }

  if (x_value > 512) {
    digitalWrite(Motor_R_dir_pin, Motor_return);
    digitalWrite(Motor_L_dir_pin, Motor_return);
    analogWrite(Motor_R_pwm_pin, x_rel_pwm);
    analogWrite(Motor_L_pwm_pin, x_rel_pwm);
  }

  if (x_value < 512) {
    digitalWrite(Motor_R_dir_pin, Motor_forward);
    digitalWrite(Motor_L_dir_pin, Motor_forward);
    analogWrite(Motor_R_pwm_pin, x_rel_pwm);
    analogWrite(Motor_L_pwm_pin, x_rel_pwm);
  }

  if (y_value > 512) {
    digitalWrite(Motor_R_dir_pin, Motor_return);
    digitalWrite(Motor_L_dir_pin, Motor_forward);
    analogWrite(Motor_R_pwm_pin, y_rel_pwm);
    analogWrite(Motor_L_pwm_pin, y_rel_pwm);
  }

    if (y_value < 512) {
    digitalWrite(Motor_R_dir_pin, Motor_forward);
    digitalWrite(Motor_L_dir_pin, Motor_return);
    analogWrite(Motor_R_pwm_pin, y_rel_pwm);
    analogWrite(Motor_L_pwm_pin, y_rel_pwm);
  }
  
}

// Motor test runs once in each direction, then the motorTestComplete flag is set to true
// This prevents any follow-up tests until the code is reuploaded
void motorTest() {

  // for now, RIGHT ENGINE goes the opposite way
  digitalWrite(Motor_R_dir_pin, Motor_return);
  digitalWrite(Motor_L_dir_pin, Motor_forward);

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
  digitalWrite(Motor_L_dir_pin, Motor_return);

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
   delay(1000);
   lcd.clear();
}

void loop() {
  /*
  if (pushFlag) {
    pushFlag = false;

    unsigned long now = millis();
    if (now - lastStateChange >= debounceMillis) {
      digitalWrite(digitalPinToInterrupt(js_key), !digitalRead(digitalPinToInterrupt(js_key)));
      lastStateChange = now;
      pushCounter++;
      //printPushCounter();
      //simpleTurns();
      //motorTest();
    }
  }
  */
  moveWithJoystick();
  printJoystickValues();
}
