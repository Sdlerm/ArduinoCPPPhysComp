#include <Arduino.h>

const int ldr = A0;
const unsigned long interval = 1000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //pinMode(ldr,OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long previousMillis = 0;
  if (currentMillis - previousMillis >= interval) 
  {
    currentMillis = previousMillis;  
  }


  double time = millis()/1000.0;
  int raw = analogRead(ldr);
  //conversion of ldr analog reading to voltage
  double ldrVoltage = raw*(5.0/1023.0);
  double ldrVoltageMv = ldrVoltage*1000;
  double ldrCurrent = ldrVoltage/10000;
  double ldrCurrentMa = ldrCurrent*1000;
  double ldrResistance = ldrVoltage/ldrCurrent;

  Serial.println("time, analog, voltage, current, resistance");
  Serial.print("Time: "); Serial.print(time);
  Serial.print("s | LDR Analog: "); Serial.print(raw);
  Serial.print(" | LDR Volage: "); Serial.print(ldrVoltageMv);
  Serial.print("mv | Current: "); Serial.print(ldrCurrentMa);
  Serial.print("mA | Resistance: "); Serial.print(ldrResistance); Serial.print(" ohms\n");

  delay(500);
}

