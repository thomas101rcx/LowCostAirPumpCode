#include <Wire.h> 

#define PUMP_A_PIN 5
#define PUMP_B_PIN 3

#define TARGET_FLOW_HIGH 0.60
#define TARGET_FLOW_LOW 0.18
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);


  pinMode(PUMP_A_PIN, OUTPUT);
  pinMode(PUMP_B_PIN, OUTPUT);
}

void writePumpA(float p) {
  p = max(p, 0);
  p = min(1, p);
  uint8_t power = p * 255;
  analogWrite(PUMP_A_PIN, power);
}


void writePumpB(float p) {
  p = max(p, 0);
  p = min(1, p);
  uint8_t power = p * 255;
  analogWrite(PUMP_B_PIN, power);
}
void loop() {

  writePumpA(1);
  writePumpB(1);
  // put your main code here, to run repeatedly:

}
