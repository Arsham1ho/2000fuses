// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 37, en = 36, d4 = 35, d5 = 34, d6 = 33, d7 = 32;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int js_x = A8, js_y = A9, js_key = 19;


// ISR STUFF IN PROGRESS, NOT FINISHED

volatile int pushState = LOW;
volatile unsigned long lastStateChange = 0;
volatile int pushCounter = 0;

void onJoystickPush() {
  if (millis() - lastStateChange > 50) {
    pushState != pushState;
    pushCounter++;
  }
}


void setup() {
  // IN PROGRESS
  Serial.begin(9600);
  pinMode(js_key, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(js_key), onJoystickPush, RISING);
  lcd.begin(20, 4);
  lcd.setCursor(0, 0);
  lcd.print("test");
}


void printJoystickValues() {
 int x_value = analogRead(js_x);
 int y_value = analogRead(js_y);
 int x_relative = (x_value * 100) / 1023;
 int y_relative = (y_value * 100) / 1023;

 Serial.println();
 
 lcd.setCursor(0, 0);
 lcd.print("x:");
 lcd.print(x_value);
 lcd.setCursor(15, 0);
 lcd.print(x_relative);
 lcd.print(" %");

lcd.setCursor(0, 1);
lcd.print("y:");
lcd.print(y_value);
lcd.setCursor(15, 1);
lcd.print(y_relative);
lcd.print(" %");
}

void loop() {
  printJoystickValues();
  delay(500);
  lcd.clear();
}
