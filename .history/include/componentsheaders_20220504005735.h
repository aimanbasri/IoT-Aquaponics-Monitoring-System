/*
Note to self: A header file in C/C++ contains:
 - Function definitions
 - Data type definitions
 - Macros
*/

// //#ifndef is often used to make header files idempotent by defining a token once the file has been included and checking that the token was not set at the top of that file.

#ifndef COMPONENTSHEADERS_h 
#define COMPONENTSHEADERS_h //Typically, header files have an include guard or a #pragma once directive to ensure that they are not inserted multiple times into a single .cpp file.

#include <Arduino.h>
#include <datalogging.h> // seperate header file for SD card methods

#include <WiFi.h>
#include <time.h> // ESP32 native time library which does graceful NTP server synchronization
const char* ntpServer = "pool.ntp.org"; //  automatically picks geographically close time zone
const long  gmtOffset_sec = 0; // UTC+0 for UK time
const int   daylightOffset_sec = 3600;  // set to 0 if no DST

// for DHT22 sensor
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h> // Unified sensor library  - Used for lux sensor, DHT22,

#include <Adafruit_TSL2591.h>
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)

// for temp sensor, using Dallas 1-Wire protocol
#include <OneWire.h>
#include <DallasTemperature.h>

const int oneWireBus = 4;   // GPIO where the DS18B20 is connected to
OneWire oneWire(oneWireBus); // Setup a oneWire instance to communicate with any OneWire devices - Temperature probe
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature sensor - Temperature probe

// Define variables for ultrasonic sensor:
long duration; // stores the time between sending and receiving the sound waves.
int distance; // used to store the calculated distance

uint32_t delayMS;

// for PH sensor
#include <DFRobot_ESP_PH.h>
#include <EEPROM.h>

DFRobot_ESP_PH ph;
#define ESPADC 4096.0   //the esp Analog Digital Convertion value
#define ESPVOLTAGE 3300 //the esp voltage supply value
#define PH_PIN 34		//the esp gpio data pin number
float voltage, phValue= 25;

// get local time for data logging
void getLocalTime(int choice){
  struct tm timeinfo; // reate a time structure (struct tm) called timeinfo that contains all the details about the time (min, sec, hour, etc…).
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  switch(choice){
    case 1: Serial.println(&timeinfo, "%H:%M:%S"); // hours, minute, seconds
    break;
    case 2: Serial.println(&timeinfo, "%d %M %Y");  // day month year
    break;
    case 3: Serial.println(&timeinfo, "%H"); // hours
    break;
    case 4: Serial.println(&timeinfo, "%M");// minutes
    break;
    case 5: Serial.println(&timeinfo, "%S"); // seconds
    break;
    default: Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S"); // 1 Jan, hours , minutes, seconds
    break;
  }
  String a  = String((&timeinfo, "%S"));
  Serial.print(a);
}
// LUX sensor methods
void displaySensorDetails(void)
{
  sensor_t sensor;
  tsl.getSensor(&sensor);   // & means passing a reference
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Sensor:       ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:   ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:    ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:    ")); Serial.print(sensor.max_value); Serial.println(F(" lux"));
  Serial.print  (F("Min Value:    ")); Serial.print(sensor.min_value); Serial.println(F(" lux"));
  Serial.print  (F("Resolution:   ")); Serial.print(sensor.resolution, 4); Serial.println(F(" lux"));  
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
  delay(500);
}

/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2591
*/
/**************************************************************************/

void configureSensor(void)
{
  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
  //tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
  tsl.setGain(TSL2591_GAIN_MED);      // 25x gain
  //tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain
  
  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  //tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
  tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)

  /* Display the gain and integration time for reference sake */  
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Gain:         "));
  tsl2591Gain_t gain = tsl.getGain();
  switch(gain)
  {
    case TSL2591_GAIN_LOW:
      Serial.println(F("1x (Low)"));
      break;
    case TSL2591_GAIN_MED:
      Serial.println(F("25x (Medium)"));
      break;
    case TSL2591_GAIN_HIGH:
      Serial.println(F("428x (High)"));
      break;
    case TSL2591_GAIN_MAX:
      Serial.println(F("9876x (Max)"));
      break;
  }
  Serial.print  (F("Timing:       "));
  Serial.print((tsl.getTiming() + 1) * 100, DEC); 
  Serial.println(F(" ms"));
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
}

void debug(){
Serial.println(F("Starting Adafruit TSL2591 Test!"));

  if (tsl.begin()) 
  {
    Serial.println(F("Found a TSL2591 sensor"));
  } 
  else 
  {
    Serial.println(F("No sensor found ... check your wiring?"));
    while (1);
  }
}

void printWaterTemperature(float watertemperature){
  Serial.print("Water temperature: ");
  Serial.print(watertemperature);
  Serial.println(F("ºC"));
}

void printAirTemperature(float airtemperature){
  Serial.print(F("Air temperature: "));
  Serial.print(airtemperature);
  Serial.println(F("°C"));
}

void printHumidity(float humidity){
  Serial.print(F("Humidity : "));
  Serial.print(humidity);
  Serial.println(F("%"));
}

void printDistance(float distance){
  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.println(" cm");
}

void printpHValue(float voltage, float phvalue){
  Serial.print("voltage:");
  Serial.println(voltage, 4);
  Serial.print("pH:");
  Serial.println(phValue, 4);
}

float measurePHValue(float watertemp){

  //voltage = rawPinValue / esp32ADC * esp32Vin
  voltage = analogRead(PH_PIN) / ESPADC * ESPVOLTAGE; // read the voltage
  phValue = ph.readPH(voltage, watertemp); // convert voltage to pH with temperature compensation
  //printpHValue(voltage, phValue);
  return phValue;
}

// debugging infrastructure; setting different DBGs true triggers prints ////
#define dbg(b, s) if(b) Serial.print(s)
#define dln(b, s) if(b) Serial.println(s)
#define startupDBG      true
#define loopDBG         true
#define monitorDBG      true
#define netDBG          true
#define miscDBG         true
#define analogDBG       true
#define otaDBG          true
#define lightDBG        true


#endif