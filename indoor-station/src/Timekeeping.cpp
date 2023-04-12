using namespace std;

#include "Timekeeping.h"

#include <Arduino.h>
#include <WiFi.h>
#include "time.h"

#define NTP_SERVER "de.pool.ntp.org"           
#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"  

void syncTimeWithServer()
{

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Time-syncing depends on a WiFi connection!");
    return;
  }
  // Init and get the time
  Serial.print("Polling NTP-Server");

  struct tm timeinfo;
  while (!getLocalTime(&timeinfo))
  {
    Serial.print(".");
    configTime(0, 0, NTP_SERVER);
  }

  setenv("TZ",TIMEZONE, 1);
  tzset();
  Serial.println();
  Serial.println("Successfully synced up time");
}