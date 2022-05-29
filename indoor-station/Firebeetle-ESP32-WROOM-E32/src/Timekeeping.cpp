using namespace std;

#include "time.h"
#include "Timekeeping.h"

#include <Arduino.h>

#include <iostream>
#include <string>
#include <cstdlib>
#include "services/WiFiService.h"

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;




void syncTimeWithServer()
{
  // NTP Time-Server
  connectWiFi();

  // Init and get the time
  Serial.print("Polling NTP-Server");

  struct tm timeinfo;
  while (!getLocalTime(&timeinfo))
  {
    Serial.print(".");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  }
  Serial.println();
  disconnectWiFi();
}