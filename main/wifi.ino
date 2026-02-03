String lastMachineState;
int lastHxTemperature = 0;
int lastSteamTemperature = 0;
long lastTimerStartMillis = 0;
bool lastTimerStarted = false;
bool lastMachineHeating = false;
bool lastMachineHeatingBoost = false;
bool machineReady = false;
bool lastMachineReady = false;
bool hasMachineBeenReady = false;
bool machineStandBy = false;
bool lastMachineStandBy = false;
bool initiated = false;
int i = 0;


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

  if (!initiated) {
    broadcastMachineState();   
    broadcastHxTemperature();
    broadcastSteamTemperature();
    // broadcastShot();
    broadcastPump();
    broadcastMachineHeating();
    broadcastMachineHeatingBoost();
    broadcastMachineReady();
    broadcastMachineStandBy();
    initiated = true;
  }

  if (lastMachineState != machineState) {
    lastMachineState = machineState;
    broadcastMachineState();   
  }

  if (lastHxTemperature != hxTemperature) {
    if (hxTemperature > 15 && hxTemperature < 115) {
      broadcastHxTemperature();
    }
    lastHxTemperature = hxTemperature;
  }

  if (lastSteamTemperature != steamTemperature) {
    if (steamTemperature > 15 && steamTemperature < 250) {
      broadcastSteamTemperature();
    }
    lastSteamTemperature = steamTemperature;
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

  if (hxTemperature >= 90) {
    machineReady = true;
    hasMachineBeenReady = true;
    machineStandBy = false;
  } else {
    machineReady = false;
  }
  if (lastMachineReady != machineReady) {
    broadcastMachineReady();
    lastMachineReady = machineReady;
  }

  if (machineState == "off"){
    hasMachineBeenReady = false;
    machineStandBy = false;
    // temp falls slowly
    if (i > 900000) { // 900000
      if (hxTemperature >= 15) {
        hxTemperature = hxTemperature - 1;
      }
      if (steamTemperature >= 15) {
        steamTemperature = steamTemperature -1;
      }
      i=0;
    }
  }
  i = (i+1) % 1000000;

  // try to identify standby mode: machine was already ready and temperature is falling below threshold
  if (hasMachineBeenReady && steamTemperature < 110) {
    machineStandBy = true;
  }
  if (machineHeating || timerStarted) {
    machineStandBy = false;
  }
  if(lastMachineStandBy != machineStandBy){
    broadcastMachineStandBy();
    lastMachineStandBy = machineStandBy;
  }

}

void broadcastMachineState() {
  if (machineState == "off") {
    client.publish("/marax/power", "off");
  } else {
    client.publish("/marax/power", "on");
  }
}

void broadcastMachineReady() {
  if (machineReady) {
    client.publish("/marax/ready", "on");
  } else {
    client.publish("/marax/ready", "off");
  }
}

void broadcastMachineStandBy() {
  if (machineStandBy) {
    client.publish("/marax/standby", "on");
  } else {
    client.publish("/marax/standby", "off");
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
