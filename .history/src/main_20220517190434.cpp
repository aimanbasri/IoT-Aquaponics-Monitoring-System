#include <componentsheaders.h>
#include <config.h>

// set up the AdafruitIO feeds
AdafruitIO_Feed *airtemp = io.feed("Ambient Temperature");
AdafruitIO_Feed *humidity = io.feed("Relative Humidity");
AdafruitIO_Feed *waterlevel = io.feed("Water Level");
AdafruitIO_Feed *watertemp = io.feed("Water Temperature");
AdafruitIO_Feed *lightlevels = io.feed("Light Level");
AdafruitIO_Feed *phlevels = io.feed("Water pH Value");

// for multitasking using timeslicing rather than delay()
unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis; // current time since the program started
unsigned long lastPostRequestTime; // time of last IO push

// for touch button
// unsigned long lastPressedTime = 0;  // the last time the output pin was toggled
// unsigned long pressedDelay = 3000;    // the debounce time; increase if the output flickers
// // Variables will change:
// int buttonState;             // the current reading from the input pin
// int lastButtonState = LOW;   // the previous reading from the input pin

//  Change here for frequency of datalog
unsigned long datalogDelay = 300000;  // logs every 5 minute
unsigned long adafruitIODelay = 3000;  // pushed to IO every 10s
unsigned long lastdatalog;
unsigned long lastIOPush; 

int loopIteration = 0;
const int LOOP_ROLLOVER = 25000000; // how many loops per action sequence
int sliceSize = 1000000;
// int TICK_DO_SOMETHING = 1;
// int TICK_DO_SOMETHING_ELSE = 9999999;

void setup() {
  Serial.begin(115200);

  startMillis = millis(); // // remember time passed since when we began 

  setupLEDS();
  setupSDCard();

  // pinMode(buttonPin, INPUT);

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    lightupLED('wifi_connected', false);
    lightupLED('connecting', true);
    delay(500);
  }

  //we are connected to WiFI/ Adafruit IO
  Serial.println();
  lightupLED('wifi_connected', true);;
  Serial.println(io.statusText());

  OLEDDisplaySetup(WIFI_SSID);
  LCDDisplaysetup();

  // set local time for rtc
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  EEPROM.begin(32);//needed to permit storage of calibration value in eeprom
	ph.begin();

  // Initialize dht device.
  dht.begin();
  sensor_t sensor; // DHT22 sensor
  sensors.begin(); // Start the DS18B20 sensor

  dht.temperature().getSensor(&sensor); // Calling the .getSensor() function will provide some basic information about the sensor . In this case, the temp sensor
  dht.humidity().getSensor(&sensor);   // Print humidity sensor details.

  // Define inputs and outputs for the ultrasonic sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
    
  /* Configure the lux sensor */
  //displaySensorDetails();  /* Display some basic information on this sensor */
  configureSensor();
 }


void loop() {

  // int reading = digitalRead(buttonPin);

  io.run(); // keeps the client connected to io.adafruit.com, and processes any incoming data.
  
  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)

  // if (reading == 1){
  //   Serial.print(currentMillis);
  //   Serial.println(" : Touched. LoRaWAN mode");
  //   // lightupLED('lorawan_connected', true);
  // }
  
  // // If the switch changed, due to noise or pressing:
  // if (reading != lastButtonState) {
  //   // reset the debouncing timer
  //   lastPressedTime = millis();
  // }

  // if ((millis() - lastPressedTime) > pressedDelay) {
  //   // whatever the reading is at, it's been there for longer than the debounce
  //   // delay, so take it as the actual current state:
    
  //   // if the button state has changed:
  //   if (reading != buttonState) {
  //     buttonState = reading;

  //     // only toggle the LED if the new button state is HIGH
  //     if (buttonState == HIGH) {
  //       Serial.print("TIME: ");
  //       Serial.print(lastPressedTime);
  //       Serial.println("- PROVISION MODE");
  //     }
  //   }
  // }

  // lastButtonState = reading;
  
  lcd.setCursor(0, 0);
  lcd.print(staticMessage);
  // LCDDisplayText(1, scrollingMessage, 250, 16);
  LCDDisplayText(0, "Air Temperature:", 250, 16);
  LCDDisplayText(1, "25.6C", 250, 16);

  // AirTemp & Humidity - Get temperature and humidity event and print its value.
  sensors_event_t event;
  // Get temperature event and print its value.
  dht.temperature().getEvent(&event); // Calling this function will populate the supplied sensors_event_t reference with the latest available sensor data
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }else{
    temp = event.temperature;
    // printAirTemperature(event.temperature);
  }

  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }else{
    //printHumidity(event.relative_humidity);
    hum = event.relative_humidity;
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

  // Simple data read example. Just use from the TSL2591_INFRARED, TSL2591_FULLSPECTRUM or 'visible' (difference between the two) channels. This can take 100-600 milliseconds! 
  uint16_t val = tsl.getLuminosity(TSL2591_VISIBLE);
  //printLuxValues(val);

 // Water temperature sensors
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  // printWaterTemperature(temperatureC);
  
  // pH Sensor
  float phvalue = measurePHValue(temperatureC);
  ph.calibration(voltage, temperatureC); // calibration process by Serial CMD

  // if(!digitalRead(BUTTON_A)) display.print("A");
  // if(!digitalRead(BUTTON_B)) display.print("B");
  // if(!digitalRead(BUTTON_C)) display.print("C");
  // delay(10);
  // yield();
  // display.display();
  
  //delay(10000);
  if ((currentMillis - lastIOPush ) > adafruitIODelay) {
    //save readings to AdafruitIO
    airtemp->save(temp);
    humidity->save(hum);
    waterlevel->save(distance);
    lightlevels->save(val);
    watertemp->save(temperatureC);
    phlevels->save(phvalue);
    Serial.println("ADAFRUIT IO UPDATED");
    lastIOPush = currentMillis;
  }
  else if ((currentMillis - lastdatalog) > datalogDelay){
    Serial.println("LOGGED TO SD CARD");
    // logs everything to SD card
    // logSDcard()
    lastdatalog = currentMillis;
  }

  if(loopIteration++ == LOOP_ROLLOVER) {

    loopIteration = 0;

    Serial.print("loopIteration rolling over; ");
    Serial.println(LOOP_ROLLOVER);
    Serial.print("loops lasted ");
    Serial.println(millis() - startMillis);
    Serial.println(" milliseconds...; rolling over");
    
  }

  }
