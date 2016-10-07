#include <Wire.h>
#include "RTClib.h"
#include <SD.h>
RTC_DS3231 RTC;
String year, month, day, second, hour, minute;
float flowrate;
int j;
int numLoops;
int high = 0;
int low = 0;
int digitalcode = 0;
int previousTime;
File myFile;
boolean resetFirstTime = false;
boolean resetRelayTime = false;
boolean completestop = false;
long firstTime = 0;
long firsttimerelay = 0;
long logTime = 0;;
long relaylogTime = 0;
String writeString;
const char * buffer = "HWAF1.txt";
void setup() {
  //set the serial's Baudrate
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  delay(200);
  digitalWrite(8, LOW);
  delay(200);
  digitalWrite(8, HIGH);
  delay(200);
  digitalWrite(8, LOW);
  delay(200);
  digitalWrite(8, HIGH);
  delay(200);
  Serial.begin(9600);
  delay(1000);

  //Initialize counter and pm total variables
  j = 1;

  //--------RTC SETUP ------------
  Serial.print("Initializing RTC Chronodot...");
  //  delay(1000);
  Wire.begin();
  RTC.begin();
  Serial.print("Doing RTC checks...");
  //  delay(1000);
  //Check if RTC is running
  Serial.print("Setting up RTC now...");
  //  delay(1000);
  DateTime now = RTC.now();
  previousTime = 0;
  DateTime compiled = DateTime(__DATE__, __TIME__);
  if (now.unixtime() < compiled.unixtime()) {
    Serial.println("RTC is older than compile time!  Updating");
    //    delay(1000);
    // RTC.adjust(DateTime(__DATE__, __TIME__));
    //uncomment if time is not already set
  }
  char datastr[100];
  //  RTC.getControlRegisterData( datastr[0]  );
  Serial.println("done");
  //  delay(1000);

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    digitalWrite(8, HIGH);
    delay(100);
    digitalWrite(8, LOW);
    delay(100);
    digitalWrite(8, HIGH);
    delay(100);
    digitalWrite(8, LOW);
    delay(100);
    digitalWrite(8, HIGH);
    delay(100);
    digitalWrite(8, LOW);
    digitalWrite(8, HIGH);
    delay(100);
    digitalWrite(8, LOW);
    delay(100);
    digitalWrite(8, HIGH);
    delay(100);
    digitalWrite(8, LOW);
    delay(100);
    digitalWrite(8, HIGH);
    delay(100);
    digitalWrite(8, LOW);
    return;
  }
  Serial.println("initialization done.");

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
  digitalWrite(8, HIGH);

  //------flow sensor setup----------

  Wire.begin();
  Wire.beginTransmission(0x49);
  Wire.write(0);
  Wire.endTransmission();
  Serial.print("setup complete");


  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  digitalWrite(6, HIGH);

}

void loop() {
  digitalWrite(8, LOW);
  if (resetRelayTime) {
    firsttimerelay = millis();
    resetRelayTime = false;
  }
  if (resetFirstTime) {
    firstTime = millis();
    resetFirstTime = false;
  }
  while (completestop) {}

  long logTime = millis();
  long relaylogTime = millis();
  //Calculate time elapsed to check if switch relay or to update sd/time
  long timeElapsed = logTime - firstTime;
  long timeElapsedRelay = relaylogTime - firsttimerelay;
  Serial.println(timeElapsed);
  if (timeElapsed > 60000) {
    delay(100);
    digitalWrite(8, HIGH);
    resetFirstTime = true;

    DateTime now = RTC.now();
    //previousTime = now.second();
    // set previous time to current time
    year = String(now.year(), DEC);
    month = String(now.month(), DEC);
    day = String(now.day(), DEC);
    hour = String(now.hour(), DEC);
    minute = String(now.minute(), DEC);
    second = String(now.second(), DEC);

    Serial.print(year);
    Serial.print('/');
    Serial.print(month);
    Serial.print('/');
    Serial.print(day);
    Serial.print(' ');

    Serial.print(hour);
    Serial.print(':');
    Serial.print(minute);
    Serial.print(':');
    Serial.print(second);
    Serial.print(" ");

    writeString = year + "/" + month + "/" + day + " " + hour + ":" + minute + ":" + second + " " + flowrate;

    sdLog(buffer, writeString);

    j = 1;
    flowrate = 0;
  }

  if (millis() > 86400000) { // if 24 hours has passed, pump will turn off and readings will stop
    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);
    completestop = true;
  }
  delay(500);

  // i2c reading
  Wire.requestFrom(0x49, 2); // request 1 byte from slave
  while (Wire.available() == 0);
  {
    Serial.print(" connected ");

    high = Wire.read(); // receive a byte as character
    low = Wire.read();
    digitalcode = (high << 8) + low;

    // to calculate the flow rate
    //flowrate = 50*((((float)digitalcode)/16384)-0.1)/0.8; //for the 50 slpm
    flowrate = .2 * ((((float) digitalcode / 16384) - 0.5) / .4);


    Serial.print("flow rate: ");
    Serial.println(flowrate);
  }
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
    digitalWrite(8, HIGH);
    delay(200);
    digitalWrite(8, LOW);
    delay(200);
    digitalWrite(8, HIGH);
    delay(200);
    digitalWrite(8, LOW);
    delay(200);
    digitalWrite(8, HIGH);
    delay(200);
    digitalWrite(8, LOW);
    delay(200);
    digitalWrite(8, HIGH);
    delay(1000);
    // if the file didn't open, print an error:
    Serial.print("error opening ");
    Serial.println(fileName);
  }
}
