#include <Wire.h> 


//The purpose of this code is to test if the Pump works as expected or not. When you run this code, both of the pumps will start from 0% duty cycle (fully off)  until 100 % duty cycle (completely on). 
//What that means is that both pumps will start from the smallest flow rate 0.0 LPM to its maximum flow rate depending on the Pump's characteristic ( usually 2 LPM).
//You should expect an increased of pump noise level.

#define PUMP_A_PIN 5
#define PUMP_B_PIN 3
float a = 0;

void setup() {
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

float calcdutycycle(float input){
  float temp = input * 0.390625;
  float answer = temp * 10000;
return answer;
 }
void loop() {
  writePumpA(a);
  Serial.print("The pump is at");
  Serial.print(calcdutycycle(a));
  Serial.println("duty cycle");
  writePumpB(a);
  a += 0.01;
  if(a >= 1){
     a = 0; 
   }
}
