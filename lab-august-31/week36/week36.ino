const int analogInPin = A1;
const int ledPin = 22;

int sensorValue = 0;
float sensorVoltage = 0;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  sensorValue = analogRead(analogInPin);
  sensorVoltage = (float) sensorValue * (5.0 / 1023.0);
  
  Serial.print("Sensor value: ");
  Serial.print(sensorValue);
  Serial.println();

  Serial.print("Voltage: ");
  Serial.print(sensorVoltage);
  Serial.print("V");
  Serial.println();

  if (sensorVoltage < 1.0) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
  
  delay(100);
}
