#include <componentsheaders.h>
#include <config.h>
//#include <display.cpp>

#define DHTPIN 21     // Digital pin connected to the DHT sensor 
#define DHTTYPE DHT22     // DHT 22 (AM2302)

DHT_Unified dht(DHTPIN, DHTTYPE);

// set up the 'airtemperature' and 'humidity' feeds
AdafruitIO_Feed *airtemperature = io.feed("airtemperature");
AdafruitIO_Feed *humidity = io.feed("humidity");
AdafruitIO_Feed *waterlevel = io.feed("Water level");
AdafruitIO_Feed *watertemperature = io.feed("watertemperature");
AdafruitIO_Feed *lightlevels = io.feed("Light levels");

// Define Trig and Echo pin for ultrasonic sensor:
#define trigPin 17
#define echoPin 16

// //pH Sensor
// #include "DFRobot_ESP_PH.h"
// #include <EEPROM.h>

// DFRobot_ESP_PH ph;
// #define ESPADC 4096.0   //the esp Analog Digital Convertion value
// #define ESPVOLTAGE 3300 //the esp voltage supply value
// #define PH_PIN 14    //the esp gpio data pin number
// float voltage, phValue;

void setup() {
  Serial.begin(115200);

  EEPROM.begin(32);//needed to permit storage of calibration value in eeprom
	ph.begin();

  // Initialize dht device.
  dht.begin();
  sensor_t sensor; // DHT22 sensor
  
  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

  sensors.begin(); // Start the DS18B20 sensor

  dht.temperature().getSensor(&sensor); // Calling the .getSensor() function will provide some basic information about the sensor . In this case, the temp sensor
  dht.humidity().getSensor(&sensor);   // Print humidity sensor details.

  // Set delay between sensor readings based on sensor details.
  //delayMS = sensor.min_delay / 1000;   // sensor->min_delay = 2000000L; // 2 seconds (in microseconds)
  delayMS = 4000000 / 1000; // 4 seconds

  // Define inputs and outputs for the ultrasonic
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
    
  /* Configure the lux sensor */
  //displaySensorDetails();  /* Display some basic information on this sensor */
  configureSensor();

  
}

void loop() {

  io.run(); // keeps the client connected to io.adafruit.com, and processes any incoming data.
  
  static unsigned long timepoint = millis();

  // AirTemp & Humidity - Get temperature and humidity event and print its value.
  sensors_event_t event;
  // Get temperature event and print its value.
  dht.temperature().getEvent(&event); // Calling this function will populate the supplied sensors_event_t reference with the latest available sensor data
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }else{
    printAirTemperature(event.temperature);
    // save temperature to Adafruit IO
    airtemperature->save(event.temperature);
  }

  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }else{
    printHumidity(event.relative_humidity);
    // save humidity to Adafruit IO
    humidity->save(event.relative_humidity);
  }

  // Ultrasonic - Get distance and humidity event and print its value.
  digitalWrite(trigPin, LOW); // Clear the trigPin by setting it LOW:
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH); // Trigger the sensor by setting the trigPin high for 10 microseconds:
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Read the echoPin. pulseIn() returns the duration (length of the pulse) in microseconds:
  duration = pulseIn(echoPin, HIGH);
  // Calculate the distance:
  distance = duration*0.034/2;  //  Formula: d = time * speed of sound / 2 . vSound = 343 m/s
  printDistance(distance);
  waterlevel->save(distance);

  // Simple data read example. Just use from the TSL2591_INFRARED, TSL2591_FULLSPECTRUM or 'visible' (difference between the two) channels.
  // This can take 100-600 milliseconds! Uncomment whichever of the following you want to read
  uint16_t x = tsl.getLuminosity(TSL2591_VISIBLE);
  Serial.print(F("[ ")); Serial.print(millis()); Serial.print(F(" ms ] "));
  Serial.print(F("Luminosity: "));
  Serial.println(x, DEC);
  lightlevels->save(x);

 // Water temperature sensors
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  printWaterTemperature(temperatureC);
  watertemperature->save(temperatureC);

  // pH Sensor
  if (millis() - timepoint > 1000U) //time interval: 1s
  {
    timepoint = millis();
    //voltage = rawPinValue / esp32ADC * esp32Vin
    voltage = analogRead(PH_PIN) / ESPADC * ESPVOLTAGE; // read the voltage
    Serial.print("voltage:");
    Serial.println(voltage, 4);
    
    // //temperature = readTemperature();  // read your temperature sensor to execute temperature compensation
    // Serial.print("temperature:");
    // Serial.print(temperature, 1);
    // Serial.println("^C");

    phValue = ph.readPH(voltage, temperatureC); // convert voltage to pH with temperature compensation
    Serial.print("pH:");
    Serial.println(phValue, 4);
  }
  ph.calibration(voltage, temperatureC); // calibration process by Serail CMD


  
  // calibration process by Serail CMD
  // if(!digitalRead(BUTTON_A)) display.print("A");
  // if(!digitalRead(BUTTON_B)) display.print("B");
  // if(!digitalRead(BUTTON_C)) display.print("C");
  // delay(10);
  // yield();
  // display.display();

  delay(5000);
  }
