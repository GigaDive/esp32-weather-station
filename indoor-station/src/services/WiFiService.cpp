using namespace std;

#include <Arduino.h>

#include <WiFi.h>
#include "services/WiFiService.h"
#include "services/Secrets.h"

const char *my_ssid = SSID;
const char *my_password = PASSWORD;

int maxRetries = 5;

bool connectWiFi()
{
    Serial.print("Connecting WiFi to ");
    Serial.println(my_ssid);
    WiFi.disconnect(false); // Reconnect the network
    WiFi.persistent(false); // Block permanent writing to flash storage. See http://www.forum-raspberrypi.de/Thread-esp8266-achtung-flash-speicher-schreibzugriff-bei-jedem-aufruf-von-u-a-wifi-begin
    WiFi.mode(WIFI_STA);

    WiFi.begin(my_ssid, my_password);
    int i;
    for (i = 0; WiFi.status() != WL_CONNECTED && i <= maxRetries; i++)
    {
        delay(1000);
        Serial.println(WiFi.status());
    } if (i > maxRetries) {
        return false;
    }
    Serial.println("");
    Serial.printf("WiFi connected. Signal: %d \n", WiFi.RSSI());
    return true;
}

void disconnectWiFi()
{
    Serial.println("Disconnecting WiFi");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    Serial.println("WiFi sucessfully disconnected");
}