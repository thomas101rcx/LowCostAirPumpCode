#include <Wire.h> 
#include "RTClib.h"
#include <SD.h>

RTC_DS3231 rtc;
String year, month, day, second, hour, minute;
File myFile;
String writeString;
const char * buffer = "HWAF1.txt";


void setup() {
  Serial.begin(9600);
  if (!rtc.begin()) {
    Serial.println("Can't fine RTC");
    while (1);
  }
  Wire.begin();
  rtc.begin();
  DateTime now = rtc.now();// Catch the time on RTC for now
  DateTime PCTime = DateTime(__DATE__, __TIME__); // Catch the time on PC for now


  //If any discrepencies , update with the time on  PC 
  if (now.unixtime() < PCTime.unixtime()) {
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
  if(SD.begin(10) == false){
    Serial.println("It didn't initialized");
   }

  //char datastr[100];

  year = String(now.year(), DEC);
  //Convert from Now.year() long to Decimal String object
  month = String(now.month(), DEC);
  day = String(now.day(), DEC);
  hour = String(now.hour(), DEC);
  minute = String(now.minute(), DEC);
  second = String(now.second(), DEC);
  String logHeader = year + "/" + month + "/" + day + " " + hour + ":" + minute + ":" + second;
  sdLog(buffer, "TSI Box 1: New Logging Session - " + logHeader);
  Serial.println(logHeader);

}

void loop() {
  DateTime now = rtc.now();

  year = String(now.year(), DEC);
  //Convert from Now.year() long to Decimal String object
  month = String(now.month(), DEC);
  day = String(now.day(), DEC);
  hour = String(now.hour(), DEC);
  minute = String(now.minute(), DEC);
  second = String(now.second(), DEC);

  writeString = year + "/" + month + "/" + day + " " + hour + ":" + minute + ":" + second + " ";

  sdLog(buffer, writeString);
  Serial.println(writeString);


  delay(1000);

}




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

//https://learn.adafruit.com/ds1307-real-time-clock-breakout-board-kit/understanding-the-code
