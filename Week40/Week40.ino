#include <LiquidCrystal.h>

//PIN definitions

const int rs = 37, en = 36, d4 = 35, d5 = 34, d6 = 33, d7 = 32;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int js_x = A8, js_y = A9, js_key = 19;

// ISR STUFF START
const int interruptPinR = 2;
const int interruptPinL = 3;
volatile int pulseCountL = 0;
volatile int pulseCountR = 0;
volatile int pulseCountAvg = 0;

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
const double distance = 7.5;
const double pulsesPerCm = 12.9;

void setup() {
  Serial.begin(9600);
  pinMode(interruptPinL, INPUT_PULLUP);
  pinMode(interruptPinR, INPUT_PULLUP);
  pinMode(js_key, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPinL), countPulseL, FALLING);
  attachInterrupt(digitalPinToInterrupt(interruptPinR), countPulseR, FALLING);
  lcd.begin(20, 4);
}
void countPulseL() {
  pulseCountL++;
}

void countPulseR() {
  pulseCountR++;
}

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

void moveSetDistance() {
  int x_value = analogRead(js_x);
  int y_value = analogRead(js_y);
  pulseCountAvg = (pulseCountL + pulseCountR) / 2;

  digitalWrite(Motor_R_dir_pin, Motor_forward);
  digitalWrite(Motor_L_dir_pin, Motor_return);
  analogWrite(Motor_R_pwm_pin, 255);
  analogWrite(Motor_L_pwm_pin, 220);

  if (pulseCountAvg >= (distance * pulsesPerCm)) {
    analogWrite(Motor_R_pwm_pin, 0);
    analogWrite(Motor_L_pwm_pin, 0);
  }
}

void printPulseCounts() {
  lcd.setCursor(0, 0);
  lcd.print("Pulse L: ");
  lcd.print(pulseCountL);
  
  lcd.setCursor(0, 1);
  lcd.print("Pulse R: ");
  lcd.print(pulseCountR);

  lcd.setCursor(0, 2);
  lcd.print("Pulse avg: ");
  lcd.print(pulseCountAvg);
}


void loop() {
  interrupts();
  moveSetDistance();
  printPulseCounts();
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
