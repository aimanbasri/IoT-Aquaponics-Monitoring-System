#include <componentsheaders.h>
#include <config.h>
//#include <display.cpp>

#define DHTPIN 21     // Digital pin connected to the DHT sensor 
#define DHTTYPE DHT22     // DHT 22 (AM2302)

DHT_Unified dht(DHTPIN, DHTTYPE);

// set up the AdafruitIO feeds
AdafruitIO_Feed *airtemperature = io.feed("Air temperatur");
AdafruitIO_Feed *humidity = io.feed("Relative humidity");
AdafruitIO_Feed *waterlevel = io.feed("Water level");
AdafruitIO_Feed *watertemperature = io.feed("Water temperature");
AdafruitIO_Feed *lightlevels = io.feed("Light level");
AdafruitIO_Feed *phlevels = io.feed("Water pH value");

// Define Trig and Echo pin for ultrasonic sensor:
#define trigPin 17
#define echoPin 16

// // for SD card
// #include <SPI.h>
// #include <SD.h>
// File myFile;

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

  //we are connected
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

//   //initialize SD card
//   while (!Serial) {
// ; // wait for serial port to connect. Needed for native USB port only
// }
// Serial.print("Initializing SD card...");
// if (!SD.begin(10)) {
// Serial.println("initialization failed!");
// while (1);
// }
// Serial.println("initialization done.");
// // open the file. note that only one file can be open at a time,
// // so you have to close this one before opening another.
// myFile = SD.open("test.txt", FILE_WRITE);
// // if the file opened okay, write to it:
// if (myFile) {
// Serial.print("Writing to test.txt...");
// myFile.println("This is a test file :)");
// myFile.println("testing 1, 2, 3.");
// for (int i = 0; i < 20; i++) {
// myFile.println(i);
// }
// // close the file:
// myFile.close();
// Serial.println("done.");
// } else {
// // if the file didn't open, print an error:
// Serial.println("error opening test.txt");
// }
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
    //printAirTemperature(event.temperature);
    // save temperature to Adafruit IO
    airtemperature->save(event.temperature);
  }

  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }else{
    //printHumidity(event.relative_humidity);
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
  // Serial.print(F("[ ")); Serial.print(millis()); Serial.print(F(" ms ] "));
  // Serial.print(F("Luminosity: "));
  // Serial.println(x, DEC);
  lightlevels->save(x);

 // Water temperature sensors
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  //printWaterTemperature(temperatureC);
  watertemperature->save(temperatureC);

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

  delay(5000);
  }
