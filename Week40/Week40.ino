#include <LiquidCrystal.h>

//PIN definitions

const int rs = 37, en = 36, d4 = 35, d5 = 34, d6 = 33, d7 = 32;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int js_x = A8, js_y = A9, js_key = 19;

// ISR STUFF START
const byte interruptPin = 3;
volatile int pulseCount = 0;

#define Motor_forward 0
#define Motor_return 1
#define Motor_L_dir_pin 7
#define Motor_R_dir_pin 8
#define Motor_L_pwm_pin 9
#define Motor_R_pwm_pin 10

// Predefined global variables

volatile bool pushFlag = false;
volatile unsigned long lastStateChange = 0;
const unsigned long debounceMillis = 50;
const double distance = 60.0;
const double pulsesPerCm = 12.9;

void setup() {
  Serial.begin(9600);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), countPulse, FALLING);
  lcd.begin(20, 4);
}
void countPulse() {
  pulseCount++;
}

// SOMETIMES JOYSTICK VALUES DO NOT CHANGE INDEPENDENTLY, BOTH ARE THE SAME VALUE
// FAULT WITH CODE OR JOYSTICK? CLARIFY
/*
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
*/


// OSKARI MOTOR HELL

void motor() {
 int x_value = analogRead(js_x);
 int y_value = analogRead(js_y);

 
   // in the original code, both tires set to spin in the same direction, didn't happen in practice
  //  LEFT ENGINES GO THE OPPOSITE WAY
    digitalWrite(Motor_R_dir_pin, Motor_return);
    digitalWrite(Motor_L_dir_pin, Motor_forward);
    analogWrite(Motor_L_pwm_pin, 255);
    analogWrite(Motor_R_pwm_pin, 255);
    delay(4000);
    analogWrite(Motor_L_pwm_pin, 0);
    analogWrite(Motor_R_pwm_pin, 0);
    
}

void exercise5() {
  int x_value = analogRead(js_x);
  int y_value = analogRead(js_y);

  digitalWrite(Motor_R_dir_pin, Motor_forward);
  digitalWrite(Motor_L_dir_pin, Motor_return);
  analogWrite(Motor_R_pwm_pin, 255);
  analogWrite(Motor_L_pwm_pin, 255);

  if (pulseCount >= (distance * pulsesPerCm)) {
    analogWrite(Motor_R_pwm_pin, 0);
    analogWrite(Motor_L_pwm_pin, 0);
  }
}


void loop() {
  exercise5();
  interrupts();
  Serial.println(pulseCount);
  delay(25);
  lcd.clear();
}

/*
            Excercise 4 questions and answers:
            How many pulses do you get for the given time period?
              - 860
            What was the distance driven in centimeters?
              - 
            Based on measured values, how many pulses per centimeter are there? 
            Is the pulse count same when driving on the table vs floor?
 */
