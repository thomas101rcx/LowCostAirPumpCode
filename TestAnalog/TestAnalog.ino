


float avgFlowlow = 0;
float avgFlowhigh = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}


void Return_Low_Flow_Rate() {
  float curFlow = 0;
  uint16_t sensorvalue = 0;

  sensorvalue = analogRead(A7);
  float Vo = sensorvalue * (5.0 / 1023.0);

  curFlow = 0.75 * (((Vo / 5) - 0.5) / 0.4);
  avgFlowlow += (curFlow - avgFlowlow) / 32;
  Serial.println(avgFlowlow);
}



void Return_High_Flow_Rate() {
  float curFlow = 0;
  uint16_t sensorvalue = 0;
  sensorvalue = analogRead(A6);
  float Vo = sensorvalue * (5.0 / 1023.0);
  Serial.println(sensorvalue);
  curFlow = 0.75 * (((Vo / 5) - 0.5) / 0.4);
  avgFlowhigh += (curFlow - avgFlowhigh) / 32;
  Serial.println(avgFlowhigh);
}

void loop() {
  // put your main code here, to run repeatedly:
  Return_Low_Flow_Rate();
  //Return_High_Flow_Rate();
}
