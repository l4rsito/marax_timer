#include <Event.h>
#include <Timer.h>
#include "secrets.h"

#define D5 (14)
#define D6 (12)
#define D7 (13)

#define PUMP_PIN D7

#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Timer.h>
#include <SoftwareSerial.h>


Adafruit_SSD1306 display(128, 64, &Wire, -1);
SoftwareSerial mySerial(D5, D6);
Timer t;

WiFiClient espClient;
PubSubClient client(espClient);

// State
int timerCount = 0;
int prevTimerCount = 0;
bool timerStarted = false;
long timerStartMillis = 0;
long timerStopMillis = 0;
long serialUpdateMillis = 0;

// Todos:
// * standbyState
// * let temperatures drop if machine is off or standby

String machineState;
bool machinePower;
bool machineHeating;
bool machineHeatingBoost;
int hxTemperature = 0;
int steamTemperature = 0;

const byte numChars = 32;
char receivedChars[numChars];

void setup() {
  Serial.begin(9600);
  if (SSID != "" && PSK != "" && MQTT_BROKER != "") {
    setupWifi();
  } else {
    WiFi.mode(WIFI_OFF);
  }

  setupTimer();
  setupSerial();
  setupDisplay();
  
  t.every(100, render);
}

void loop() {
  t.update();
  updateTimer();
  updateSerial();
  
  if (SSID != "" && PSK != "" && MQTT_BROKER != "") {
    updateWifi();
  }
}
