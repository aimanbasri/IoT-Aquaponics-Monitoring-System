#ifndef DATALOGGING_h 
#define COMPONENTSHEADERS_h //Typically, header files have an include guard or a #pragma once directive to ensure that they are not inserted multiple times into a single .cpp file.

#include <Arduino.h>
#include <config.h>
// Adapted from Rui Santos from randomnerdtutorials.com

// Libraries for SD card
#include "FS.h"
#include "SD.h"
#include <SPI.h>

// Define CS pin for the SD card module
#define SD_CS 15

String dataMessage;

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

void setupSDCard() {

  // Initialize SD card
  SD.begin(SD_CS);  
  if(!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }

  // If the data.txt file doesn't exist
  // Create a file on the SD card and write the data labels
  File file = SD.open("/aquaponics_datalog.txt");
  if(!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/aquaponics_datalog.txt", "Reading ID, Date, Hour, Temperature \r\n");
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();

  getReadings();
  //dayStamp = getLocalTime(2);
  //timeStamp = getLocalTime(1);
  
  //logSDCard();
  
}

// Function to get temperature
void getReadings(){

}

// get local time for data logging
void getLocalTime(int choice){
  struct tm timeinfo;
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
  
}

// Write the sensor readings on the SD card
// void logSDCard() {
//   dataMessage = String(readingID) + "," + String(dayStamp) + "," + String(timeStamp) + "," + 
//                 String(temperature) + "\r\n";
//   Serial.print("Save data: ");
//   Serial.println(dataMessage);
//   appendFile(SD, "/aquaponics_datalog.txt", dataMessage.c_str());
// }

// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}


#endif