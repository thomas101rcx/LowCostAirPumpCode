#include <Wire.h> 
#include "RTClib.h"
#include <SD.h> 
#include <stdint.h>

#define PUMP_A_PIN 5 // 0.6LPM
#define PUMP_B_PIN 3 // 0.2LPM

//PumpA is high, PumpB is low
//Any variables that ends with a high means 0.6LPM
//Any variables that ends with a low  means 0.2LPM

#define TARGET_FLOW_HIGH 0.55
#define TARGET_FLOW_LOW 0.18

//Change the default values base on the reading from the calibrator

float avgFlowhigh = 0;
float avgFlowlow = 0;
static uint32_t counter = 1; 
static uint32_t restartcounter = 0;
int timecounter = 0;
int runningtime = 0;
int timeleft = 0;

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
  else{
    Serial.println("RTC initialized successfully");  
  }

  DateTime now = rtc.now(); // Catch the time on RTC for now
  DateTime PCTime = DateTime(__DATE__, __TIME__); // Catch the time on PC for now

  //If any discrepencies , update with the time on PC 
  //Manually change this code when the timezone is different 
  // Uncomment the rtc.adjust(DateTime(__DATE__, __TIME__));
  // Upload it again to Arduino
  // Check if the time is correct
  // Comment out rtc.adjust(DateTime(__DATE__, __TIME__)); again
  // Upload the entire code again
  if (now.unixtime() < PCTime.unixtime()) {
    //rtc.adjust(DateTime(__DATE__, __TIME__));
  }
  Wire.begin();
  rtc.begin();

  //Pump setup

  pinMode(PUMP_A_PIN, OUTPUT);
  pinMode(PUMP_B_PIN, OUTPUT);

  //SD card setup
  
  if (SD.begin(10) == false) {
    Serial.println("It didn't initialized");
  }else{
    Serial.println("SD card Initialized successfully");
  }

  //Writes in The inital starting time (Tstart) to SD card
  
  year = String(now.year(), DEC);
  month = String(now.month(), DEC);
  day = String(now.day(), DEC);
  hour = String(now.hour(), DEC);
  minute = String(now.minute(), DEC);
  second = String(now.second(), DEC);
  String logHeader = year + "/" + month + "/" + day + " " + hour + ":" + minute + ":" + second;
  sdLog(buffer, "HighFlowRate_0.6: New Logging Session - " + logHeader);
  sdLog(buffer1, "LowFlowRate_0.2 : New Logging Session - " + logHeader);
  Serial.println(logHeader);

  //Check if the power has been cut off for the past 1.5 hours
  if(sdRead(buffer1) < 90 || sdRead(buffer) < 90){
    int runningtime = sdRead(buffer1);
    int timeleft = 90 - runningtime;
  }
    
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

void Return_Low_Flow_Rate() {
  float curFlow = 0;
  uint16_t sensorvalue = 0;
  sensorvalue = analogRead(A7);
  float Vo = sensorvalue * (5.0 / 1023.0);
  curFlow = 0.75 * (((Vo / 5) - 0.5) / 0.4);
  avgFlowlow += (curFlow - avgFlowlow) / 32;
  //Serial.println(avgFlowlow);
}

void Return_High_Flow_Rate() {
  float curFlow = 0;
  uint16_t sensorvalue = 0;
  sensorvalue = analogRead(A6);
  float Vo = sensorvalue * (5.0 / 1023.0);
 // Serial.println(sensorvalue);
  curFlow = 0.75 * (((Vo / 5) - 0.5) / 0.4);
  avgFlowhigh += (curFlow - avgFlowhigh) / 32;
  //Serial.println(avgFlowhigh);
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
    digitalWrite(13, HIGH);
    delay(300);
    digitalWrite(13, LOW);
    delay(300);
  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening ");
    Serial.println(fileName);
  }
}

//Read the last tiemcounter from SD card when power is shut off
int sdRead(const char *fileName){
  File myfile = SD.open(fileName);
  int timecount = 0 ;
  int timecountarray [20];
  if(myfile){
    while (myfile.available()){
    String line =  myfile.readStringUntil('\n');
    int spaceIndex = line.indexOf(' ');
    // Search for the next space just after the first
    int secondspaceIndex = line.indexOf(' ', spaceIndex + 1);
    int thirdspaceIndex  = line.indexOf(' ', secondspaceIndex + 1 );
    String firstValue = line.substring(0, spaceIndex);
    String secondValue = line.substring(spaceIndex+1, secondspaceIndex);
    String thirdValue = line.substring(secondspaceIndex+1, thirdspaceIndex); // To the end of the string
    String fourthValue = line.substring(thirdspaceIndex);
    timecount = fourthValue.toInt();
    timecountarray[0] = timecount;
    }
    myfile.close();      
   }
   return timecountarray[0];
 }

void loop(){
  

  static float pwmhigh = 0.5; // For 0.6 LPM
  static float pwmlow = 0.5; // For 0.2 LPM
  //The reason to set pwmhigh and pwmlow is to start with a initial value for the feedback loop to either add up the error or subtract the error, reaching the desire power output -> desire flowrate.
  Return_High_Flow_Rate();
  Return_Low_Flow_Rate();

  if (millis() >= 0)
  {
    float errorHigh = TARGET_FLOW_HIGH - avgFlowhigh;
    float errorLow = TARGET_FLOW_LOW - avgFlowlow;
    pwmhigh = max(pwmhigh, 0); // For pwmhigh < 0
    pwmhigh = min(1, pwmhigh); // For pwmhigh > 1
    pwmlow = max(pwmlow, 0); // For pwmhigh < 0 
    pwmlow = min(1, pwmlow); // For pwmhigh > 1
    pwmhigh += errorHigh / 100;
    pwmlow += errorLow / 100;
    // 100 is a time constant that tells us how precise do we want to get to the desire flow rate/ power output.
    writePumpA(pwmhigh);
    writePumpB(pwmlow);
  }
  
  //Every minute log the data into SD card , "Time + Flowrate + Counter" for desire time,  ex: 1.5 hours
  
  if(millis() % 60000 == 0 && avgFlowlow >= 0.1 && avgFlowhigh >= 0.1) { 
     
    DateTime now = rtc.now();
    year = String(now.year(), DEC);
    //Convert from Now.year() long to Decimal String object
    month = String(now.month(), DEC);
    day = String(now.day(), DEC);
    hour = String(now.hour(), DEC);
    minute = String(now.minute(), DEC);
    second = String(now.second(), DEC);
    writeString = year + "/" + month + "/" + day + " " + hour + ":" + minute + ":" + second + " ";
    sdLog(buffer1, writeString + avgFlowlow + " " + counter);
    sdLog(buffer, writeString + avgFlowhigh + " " + counter);
    Serial.println(writeString);
    counter ++;
    Serial.println(counter);
  }

  // run from when the power shut off
  
  if(timeleft != 0 && millis() % 60000 ==0 && avgFlowlow >= 0.1 && avgFlowhigh >=0.1){
    restartcounter = runningtime;
    DateTime now = rtc.now();
    year = String(now.year(), DEC);
    //Convert from Now.year() long to Decimal String object
    month = String(now.month(), DEC);
    day = String(now.day(), DEC);
    hour = String(now.hour(), DEC);
    minute = String(now.minute(), DEC);
    second = String(now.second(), DEC);
    writeString = year + "/" + month + "/" + day + " " + hour + ":" + minute + ":" + second + " ";
    sdLog(buffer1, writeString + avgFlowlow + " " + restartcounter);
    sdLog(buffer, writeString + avgFlowhigh + " " + restartcounter);
    Serial.println(writeString);
    restartcounter ++;
   }

  //Restart counter will stop once it reaches 90 minutes
  
  if(restartcounter > 90){
   writePumpA(0);
   writePumpB(0); 
  }

  //Turn off pumps around 1.5 hours = 5,400,000 miliseconds
  
  if (counter > 90) {  
    writePumpA(0);
    writePumpB(0);
    
  }
}
