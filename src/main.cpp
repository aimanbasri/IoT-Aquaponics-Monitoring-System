#include <componentsheaders.h>

#define DHTPIN 21     // Digital pin connected to the DHT sensor 
#define DHTTYPE DHT22     // DHT 22 (AM2302)

DHT_Unified dht(DHTPIN, DHTTYPE);
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)

OneWire oneWire(oneWireBus); // Setup a oneWire instance to communicate with any OneWire devices - Temperature probe
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature sensor - Temperature probe
const int oneWireBus = 4;   // GPIO where the DS18B20 is connected to

uint32_t delayMS;

// Define Trig and Echo pin for ultrasonic sensor:
#define trigPin 17
#define echoPin 16

void setup() {
  // Initialize dht device.
  dht.begin();
  sensor_t sensor; // DHT22 sensor
  
  sensors.begin(); // Start the DS18B20 sensor

  //dht.temperature().getSensor(&sensor); // Calling the .getSensor() function will provide some basic information about the sensor . In this case, the temp sensor
  //dht.humidity().getSensor(&sensor);   // Print humidity sensor details.

  // Set delay between sensor readings based on sensor details.
  //delayMS = sensor.min_delay / 1000;   // sensor->min_delay = 2000000L; // 2 seconds (in microseconds)
  delayMS = 4000000 / 1000; // 4 seconds

  // Define inputs and outputs for the ultrasonic
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
    
  /* Configure the lux sensor */
  //displaySensorDetails();  /* Display some basic information on this sensor */
  configureSensor();

  Serial.begin(115200);
}

void loop() {

  // AirTemp & Humidity - Get temperature and humidity event and print its value.
  sensors_event_t event;
  // Get temperature event and print its value.
  dht.temperature().getEvent(&event); // Calling this function will populate the supplied sensors_event_t reference with the latest available sensor data
  (isnan(event.temperature)) ? Serial.println(F("Error reading temperature!")); : printAirTemperature(event.temperature);}
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  (isnan(event.relative_humidity)) ? Serial.println(F("Error reading humidity!")); : printHumidity(event.relative_humidity)

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

  // Simple data read example. Just use from the TSL2591_INFRARED, TSL2591_FULLSPECTRUM or 'visible' (difference between the two) channels.
  // This can take 100-600 milliseconds! Uncomment whichever of the following you want to read
  uint16_t x = tsl.getLuminosity(TSL2591_VISIBLE);
  Serial.print(F("[ ")); Serial.print(millis()); Serial.print(F(" ms ] "));
  Serial.print(F("Luminosity: "));
  Serial.println(x, DEC);

 // Temperature sensors
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  printWaterTemperature(temperatureC);

  delay(1000);
  
  }
