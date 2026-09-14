// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 37, en = 36, d4 = 35, d5 = 34, d6 = 33, d7 = 32;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int js_x = A8, js_y = A9, js_key = 19;


// ISR STUFF FINISHED, BUT JANKY

volatile bool pushFlag = false;
volatile unsigned long lastStateChange = 0;
const unsigned long debounceMillis = 50;
volatile unsigned int pushCounter = 0;

void onJoystickPush() {
  pushFlag = true;
}


void setup() {
  // IN PROGRESS
  pinMode(js_key, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(js_key), onJoystickPush, FALLING);
  lcd.begin(20, 4);
}


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
    if (now - lastStateChange >= debounceMillis) {
      digitalWrite(digitalPinToInterrupt(js_key), !digitalRead(digitalPinToInterrupt(js_key)));
      lastStateChange = now;
      pushCounter++;
      printPushCounter();
    }
  }
  printJoystickValues();
  delay(100);
  lcd.clear();
}
