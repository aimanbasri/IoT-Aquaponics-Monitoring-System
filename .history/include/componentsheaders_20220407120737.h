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

// for OLED display
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define BUTTON_A 15
#define BUTTON_B 32
#define BUTTON_C 14
#define WIRE Wire

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &WIRE);

// For LCD Display
#include <LiquidCrystal_I2C.h>
//define I2C address......
LiquidCrystal_I2C lcd(0x27,16,2);

// Define variables for ultrasonic sensor:
long duration; // stores the time between sending and receiving the sound waves.
int distance; // used to store the calculated distance

uint32_t delayMS;

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