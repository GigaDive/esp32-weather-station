using namespace std;

#include "Forecast.h"

#include <Arduino.h>
#include <WiFi.h>


void syncDWDForecast()
{
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Getting the forecast depends on a WiFi connection!");
    return;
  }

  Serial.println("I am a stub, so nothing happens yet");
}