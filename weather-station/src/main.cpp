#include <Arduino.h>
#include "wifiManager.h"
#include "mqttInterface.h"

void setup()
{
    setupWiFiManager();
    setupMQTT();
}

void loop()
{
    loopWiFiManager();
    loopMQTT();
}