/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#ifdef ESP32
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#else
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#endif
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = A1;
const int LOADCELL_SCK_PIN = A0;

HX711 scale;

/*#include <SPI.h>
  #define BME_SCK 18
  #define BME_MISO 19
  #define BME_MOSI 23
  #define BME_CS 5*/

//Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

// Replace with your network credentials
const char* ssid = "Verizon-MiFi8800L-4AA2";
const char* password = "640b8aaf";

float t;
float t_max=0.;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readBME280Temperature() {
  // Read temperature as Celsius (the default)
  //float t = bme.readTemperature();
  //t = 22.3+random(-5,5);
  t =  scale.get_units();
  if(t>t_max) t_max=t;
  // Convert temperature to Fahrenheit
  //t = 1.8 * t + 32;
  if (isnan(t)) {
    Serial.println("Failed to read from BME280 sensor!");
    return "";
  }
  else {
    Serial.println(t);
    return String(t);
  }
}

String readBME280Humidity() {
  //float h = bme.readHumidity();
  //float h = 34.2;
  
    Serial.println(t_max);
    return String(t_max);
  
}

String readBME280Pressure() {
  //float p = bme.readPressure() / 100.0F;
  float p = 24.1;
  if (isnan(p)) {
    Serial.println("Failed to read from BME280 sensor!");
    return "";
  }
  else {
    Serial.println(p);
    return String(p);
  }
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(9600);

scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();  
  
  /*
    bool status;
    // default settings
    // (you can also pass in a Wire library object like &Wire2)
    status = bme.begin(0x76);
    if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
    }
  */

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html");
  });
  server.on("/force", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", readBME280Temperature().c_str());
  });
  server.on("/max", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", readBME280Humidity().c_str());
  });
  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", readBME280Pressure().c_str());
  });

  // Start server
  server.begin();
}

void loop() {

}
