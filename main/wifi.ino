String lastMachineState;
int lastHxTemperature = 0;
int lastSteamTemperature = 0;
long lastTimerStartMillis = 0;
bool lastTimerStarted = false;
bool lastMachineHeating = false;
bool lastMachineHeatingBoost = false;

void setupWifi() { 
  WiFi.begin(SSID, PSK);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
  client.setServer(MQTT_BROKER, 1883);
}

void updateWifi() {
  if (!client.connected()) {
    while (!client.connected()) {
      if ( MQTT_USER != "" && MQTT_PASSWORD != "" ) {
        client.connect("marax", MQTT_USER, MQTT_PASSWORD);
        delay(100);
        }
      else {
        client.connect("marax");
        delay(100);
      }
    }
  }
  
  client.loop();

  if (lastMachineState != machineState) {
    lastMachineState = machineState;
    broadcastMachineState();   
  }

  if (lastHxTemperature != hxTemperature) {
    lastHxTemperature = hxTemperature;
    if (hxTemperature > 15 && hxTemperature < 115 && abs(hxTemperature - lastHxTemperature) < 3) {
    broadcastHxTemperature();}
  }

  if (lastSteamTemperature != steamTemperature) {
    lastSteamTemperature = steamTemperature;
    if (steamTemperature > 15 && steamTemperature < 250 && abs(steamTemperature - lastSteamTemperature) < 3) {
    broadcastSteamTemperature();}
  }

  if (lastTimerStartMillis != timerStartMillis && ((millis() - timerStartMillis ) / 1000) > 15 && timerStarted == false && timerCount > 0) {
    lastTimerStartMillis = timerStartMillis;
    broadcastShot();
  }

  if (lastTimerStarted != timerStarted) {
    lastTimerStarted = timerStarted;
    broadcastPump();
  }
  
  if (lastMachineHeating != machineHeating) {
    lastMachineHeating = machineHeating;
    broadcastMachineHeating();
  }

  if (lastMachineHeatingBoost != machineHeatingBoost) {
    lastMachineHeatingBoost = machineHeatingBoost;
    broadcastMachineHeatingBoost();
  }
}

void broadcastMachineState() {
  if (machineState == "off") {
    client.publish("/marax/power", "off");
  } else {
    client.publish("/marax/power", "on");
  }
}

void broadcastHxTemperature() {
  client.publish("/marax/hx", String(hxTemperature).c_str());
}

void broadcastSteamTemperature() {
  client.publish("/marax/steam", String(steamTemperature).c_str());
}

void broadcastShot() {
  client.publish("/marax/shot", String(timerCount).c_str());
}

void broadcastPump() {
  if (timerStarted) {
    client.publish("/marax/pump", "on");
  } else {
    client.publish("/marax/pump", "off");
  }
}

void broadcastMachineHeating () {
  if (machineHeating) {
    client.publish("/marax/machineheating", "on");
  } else {
    client.publish("/marax/machineheating", "off");
  }
}

void broadcastMachineHeatingBoost () {
  if (machineHeatingBoost) {
    client.publish("/marax/machineheatingboost", "on");
  } else {
    client.publish("/marax/machineheatingboost", "off");
  }
}
