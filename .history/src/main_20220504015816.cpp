#include <componentsheaders.h>
#include <config.h>
//#include <display.cpp>

// set up the AdafruitIO feeds
AdafruitIO_Feed *airtemp = io.feed("Ambient Temperature");
AdafruitIO_Feed *humidity = io.feed("Relative Humidity");
AdafruitIO_Feed *waterlevel = io.feed("Water Level");
AdafruitIO_Feed *watertemp = io.feed("Water Temperature");
AdafruitIO_Feed *lightlevels = io.feed("Light Level");
AdafruitIO_Feed *phlevels = io.feed("Water pH Value");


void setup() {
  Serial.begin(115200);

  setupSDCard();

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  //we are connected
  Serial.println();
  Serial.println(io.statusText());

  // set local time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  EEPROM.begin(32);//needed to permit storage of calibration value in eeprom
	ph.begin();

  // Initialize dht device.
  dht.begin();
  sensor_t sensor; // DHT22 sensor
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
    airtemp->save(event.temperature);
    logSDCard(event.temperature);
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
  uint16_t val = tsl.getLuminosity(TSL2591_VISIBLE);
  Serial.print(F("[ ")); Serial.print(millis()); Serial.print(F(" ms ] "));
  Serial.print(F("Luminosity: "));
  Serial.println(val, DEC);
  lightlevels->save(val);

 // Water temperature sensors
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  printWaterTemperature(temperatureC);
  watertemp->save(temperatureC);

  // pH Sensor
  float phvalue = measurePHValue(temperatureC);
  ph.calibration(voltage, temperatureC); // calibration process by Serial CMD
  // save pH value to Adafruit IO
  phlevels->save(phvalue);

  // if(!digitalRead(BUTTON_A)) display.print("A");
  // if(!digitalRead(BUTTON_B)) display.print("B");
  // if(!digitalRead(BUTTON_C)) display.print("C");
  // delay(10);
  // yield();
  // display.display();
  //printLocalTime();
  
  delay(10000);
  }
