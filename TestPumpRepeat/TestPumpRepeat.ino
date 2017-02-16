

float temp = 0;
void setup() {
  Serial.begin(9600);
}

void writePumpA(float p) {
  p = max(p, 0);
  p = min(1, p);
  uint8_t power = p * 255;
  analogWrite(A6, power);
  
}

void loop() {
 Serial.println("1");
 if(temp <= 1){
 writePumpA(temp);
 temp += 0.01;
 delay(100);
 }
 else{
 temp = 0; 
 }
}
