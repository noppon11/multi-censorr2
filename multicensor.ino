#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <math.h>
#include "RTClib.h"
#include "TinyGPS++.h"
#include "SoftwareSerial.h"
int BH1750address = 0x23;
byte buff[2];
RTC_DS1307 RTC; 
const int chipSelect = 4;
String msg;
String fileName;
File myFile;

void setup() {
  Serial.begin(9600);
  bool sd = setupSD(); 
  Serial.println(sd);
  if(sd){
    Serial.println("sd setup success!!");
  }else{
    Serial.println("sd setup fail!!");
    while(1);
  }
}
      
void loop() {
 String light = lightSensor();
 String timee = getTime();
 String gps = getGps();
 msg = light + ":" + timee + ":" + gps;
 fileName = "bb.txt";
  bool wr = writeData(msg,fileName,myFile);
  if(wr){
    Serial.println("write success");
    Serial.println(msg);
    bool rd = readData(fileName);
  }else{
    Serial.println("write failed");
    while(1);
  }
}

String lightSensor(){
  Wire.begin();
  int i;
  uint16_t val=0;
  BH1750_Init(BH1750address);
  delay(200);
  if(2==BH1750_Read(BH1750address))
  {
    val=((buff[0]<<8)|buff[1])/1.2;
    Serial.print(val,DEC);     
    Serial.println("lux"); 
  }
  delay(150);
  return lightSensor();
}

int BH1750_Read(int address) 
{
  int i=0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while(Wire.available()) 
  {
    buff[i] = Wire.read();  
    i++;
  }
  Wire.endTransmission();  
  return i;
}

void BH1750_Init(int address) 
{
  Wire.beginTransmission(address);
  Wire.write(0x10);//1lx reolution 120ms
  Wire.endTransmission();
}

String getTime(){
  Wire.begin(); 
  RTC.begin();  
  RTC.adjust(DateTime(__DATE__, __TIME__));  
  Serial.print('Time and date set');
  DateTime now = RTC.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  delay(1000);
  return getTime();
}

String getGps(){
SoftwareSerial serial_connection(9,10); 
TinyGPSPlus gps;
serial_connection.begin(9600);
Serial.println("GPS Start");
while(serial_connection.available())
  {
    gps.encode(serial_connection.read());
  }
  if(gps.location.isUpdated())
  {
    Serial.println("Satellite Count:");
    Serial.println(gps.satellites.value());
    Serial.println("Latitude:");
    Serial.println(gps.location.lat(), 6);
    Serial.println("Longitude:");
    Serial.println(gps.location.lng(), 6);
    Serial.println("Speed MPH:");
    Serial.println(gps.speed.mph());
    Serial.println("Altitude Feet:");
    Serial.println(gps.altitude.feet());
    Serial.println("");
  }
  return getGps();
}

bool setupSD(){ // setup SD card module
  while (!Serial) {}
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    return false;
  }
  return true;
}

bool writeData(String msg, String fileName, File myFile){
    myFile = SD.open(fileName,FILE_WRITE);
    if(myFile){
      myFile.println(msg);
      myFile.close();
      return true;
    }else{
      return false;
    }
}

bool readData(String fileName){
  myFile = SD.open(fileName);
  if(myFile){
    Serial.println(fileName + ":");
    while(myFile.available()){
      Serial.write(myFile.read());
    }
    myFile.close();
    return true;
  }else{
    return false;
  }
}
