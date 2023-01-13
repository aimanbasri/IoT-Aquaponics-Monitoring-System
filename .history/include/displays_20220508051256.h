#ifndef DISPLAYS_h 
#define DISPLAYS_h //Typically, header files have an include guard or a #pragma once directive to ensure that they are not inserted multiple times into a single .cpp file.

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

// for OLED 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// #define BUTTON_A 15
// #define BUTTON_B 32
// #define BUTTON_C 14
#define WIRE Wire

//for LCD display
#include <LiquidCrystal_I2C.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &WIRE);
LiquidCrystal_I2C lcd(0x27, 16, 2); // 16x2 screen

String staticMessage = "I2C LCD Tutorial";
String scrollingMessage = "Welcome to Microcontrollerslab! This is a scrolling message.";

void OLEDDisplaySetup(){

  Serial.println("OLED FeatherWing test");
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32

  Serial.println("OLED begun");

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  display.display();

  Serial.println("IO test");

  // text display tests
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("Connecting to SSID\n'adafruit':");
  display.print("connected!");
  display.println("IP: 10.0.1.23");
  display.println("Sending val #0");
  display.setCursor(0,0);
  display.display(); // actually display all of the above
}

void LCDDisplaysetup(){

lcd.init(); 
lcd.backlight(); // use to turn on and turn off LCD back light
}

void LCDDisplayTExt(int row, String message, int delayTime, int totalColumns) {
  for (int i=0; i < totalColumns; i++) {
    message = " " + message;  
  } 
  message = message + " "; 
  for (int position = 0; position < message.length(); position++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(position, position + totalColumns));
    delay(delayTime);
  }
}

#endif