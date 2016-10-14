#include <Wire.h> 
#include "RTClib.h"
#include <SD.h>
#include <stdint.h>

#define PUMP_A_PIN 5
#define PUMP_B_PIN 3

#define FLOW_A_ADDR 0x49

//Any variables that ends with a high means 0.6LPM
//Any variables that ends with a low  means 0.2LPM

#define TARGET_FLOW_HIGH 0.60
#define TARGET_FLOW_LOW 0.20

//include real time clock in the future 

//Change the defulat values base on the reading from the calibrator

float avgFlowhigh = 0;
float avgFlowlow = 0;


RTC_DS3231 rtc;


String year, month, day, second, hour, minute;
File myFile;
String writeString;
const char * buffer = "HighFlow.txt";
const char * buffer1 = "LowFlow.txt";
void setup() {
  
  Serial.begin(9600);
  
  //RTC setup
  
  if (!rtc.begin()) {
    Serial.println("Can't fine RTC");
    while (1);
  }

  DateTime now = rtc.now();// Catch the time on RTC for now
  DateTime PCTime = DateTime(__DATE__, __TIME__); // Catch the time on PC for now

  //If any discrepencies , update with the time on PC 
  
  if (now.unixtime() < PCTime.unixtime()) {
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
  Wire.begin();
  rtc.begin();

  //Flow meter high setup

  Wire.beginTransmission(0x49);
  Wire.endTransmission();

  //Pump setup
  
  pinMode(PUMP_A_PIN, OUTPUT);
  pinMode(PUMP_B_PIN, OUTPUT);

  //SD card setup
   if(SD.begin(10) == false){
    Serial.println("It didn't initialized");
   }

  //Writes in Tstart to SD card
  year = String(now.year(), DEC);
  //Convert from Now.year() long to Decimal String object
  month = String(now.month(), DEC);
  day = String(now.day(), DEC);
  hour = String(now.hour(), DEC);
  minute = String(now.minute(), DEC);
  second = String(now.second(), DEC);
  String logHeader = year + "/" + month + "/" + day + " " + hour + ":" + minute + ":" + second;
  sdLog(buffer, "TSI Box 1: New Logging Session - " + logHeader);
  
  sdLog(buffer1, "TSI Box 1 : New Logging Session - " + logHeader);
  
  Serial.println(logHeader);

}

//Writes to pump A, takes a float from 0 to 1
void writePumpA(float p) {
  p = max(p, 0);
  p = min(1, p);
  uint8_t power = p * 255;
  analogWrite(PUMP_A_PIN, power);
}

//Writes to pump B, takes a float from 0 to 1
void writePumpB(float p) {
  p = max(p, 0);
  p = min(1, p);
  uint8_t power = p * 255;
  analogWrite(PUMP_B_PIN, power);
}

//Gets the flow readings through I2C protocal (2 bytes) and return the actual flow rate
void Return_High_Flow_Rate() {
  float curFlow = 0;
  uint8_t high = 0;
  uint16_t digitalcode = 0;

  Wire.requestFrom(0x49, 2);
  high = Wire.read();
  digitalcode = (high << 8) + Wire.read();
  
  curFlow = 0.750 * ((((float) digitalcode / 16384.0) - 0.5) / .4);
  avgFlowhigh += (curFlow - avgFlowhigh) / 64;
  //Get the average by dividing a number, how many data points do we need to take the average
}

//Gets the flow readings through I2C protocal (2 bytes) and return the actual flow rate
void Return_Low_Flow_Rate(){
  float curFlow = 0;
  uint16_t sensorvalue = 0;

  sensorvalue = analogRead(A7);
  float Vo = sensorvalue *(5.0/ 1023.0);

  curFlow = 0.75*(((Vo/5) - 0.5) / 0.4);
  avgFlowlow += (curFlow - avgFlowlow) / 32;
}



//Writes into SD card
void sdLog(const char * fileName, String stringToWrite) {
  File myFile = SD.open(fileName, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to ");
    Serial.print(fileName);
    Serial.print("...");
    myFile.println(stringToWrite);
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening ");
    Serial.println(fileName);
  }
}



void loop() {
  static uint16_t i = 0;
  static float pwmhigh = 0.5; // For 0.6 LPM
  static float pwmlow = 0.5;  // For 0.2 LPM
  static uint64_t j = 0;

  Return_High_Flow_Rate();
  Return_Low_Flow_Rate();

  if(i++ % 20 == 0)
  //Every 20 msec update the pump PWM
  {
    float errorHigh = TARGET_FLOW_HIGH - avgFlowhigh;
    float errorLow =  TARGET_FLOW_LOW  - avgFlowlow;

    pwmhigh = max(pwmhigh, 0);
    pwmhigh = min(1, pwmhigh);

    pwmlow = max(pwmlow, 0);
    pwmlow = min(1, pwmlow);
    
    pwmhigh += errorHigh / 100;
    pwmlow  += errorLow / 100;
   // 100 is the time constant

    writePumpA(pwmhigh);
    writePumpB(pwmlow);
        
   // Serial.println(avgFlowlow);
   //Serial.print(pwmhigh*1000);
   //Serial.print(" ");
   //Serial.println(avgFlowhigh*1000);
  }
  j++;
  //Everysecond log the data into SD card , Time + Flowrate
  
  if(j % 60000 == 0){
  DateTime now = rtc.now();
  year = String(now.year(), DEC);
  //Convert from Now.year() long to Decimal String object
  month = String(now.month(), DEC);
  day = String(now.day(), DEC);
  hour = String(now.hour(), DEC);
  minute = String(now.minute(), DEC);
  second = String(now.second(), DEC);

  writeString = year + "/" + month + "/" + day + " " + hour + ":" + minute + ":" + second + " ";

  sdLog(buffer1, writeString + avgFlowlow);
  sdLog(buffer, writeString + avgFlowhigh);
    
  }

  // Turn off the pump according to specification
  delay(1);// Every 1 milisec update the avgflow 
}
