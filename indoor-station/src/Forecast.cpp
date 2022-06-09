using namespace std;

#include "Forecast.h"
#include "services/DateAdder.h"
#include "services/IconMapper.h"
#include "services/WindKmhToBft.h"
#include "services/WiFiService.h"

#include <Arduino.h>
#include <stdio.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Calling the brightsky api only works over https, so we need a certificate to check against
const static char *root_ca PROGMEM =
    "-----BEGIN CERTIFICATE-----\n"
    "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\n"
    "-----END CERTIFICATE-----"; // ISRG Root X1 - Self signed, valid until Jun. 4 11:04:38 2035 GMT

struct forecast_data
{
  char todayIcon;
  float todayAvgTemp;
  float todayMaxTemp;
  float todayMinTemp;
  float todayRainAmt;
  float todayWindSpeed;
  int todaySunHrs;
  int todaySunMin;
  float todayOvercastPercent;

  char tomorrowIcon;
  float tomorrowAvgTemp;

  char overmorrowIcon;
  float overmorrowAvgTemp;

  char overOvermorrowIcon;
  float overOvermorrowAvgTemp;
};
forecast_data forecastData;

struct brightsky_response
{
  const char *weather_item_icon[24];
  float weather_item_temperature[24];
  float weather_item_precipitation[24];
  float weather_item_wind_speed[24];
  int weather_item_sunshine[24];
  int weather_item_cloud_cover[24];
};
brightsky_response brightskyResponse;

void callBrightSkyApi(Date date)
{

  HTTPClient http;
  char request[100];
  sprintf(request, "https://api.brightsky.dev/weather?lat=XXX&lon=XXX&date=%d-%d-%d", date.year, date.month, date.day); 
  Serial.printf("Polling api at address %s...", request);

  // Make the request for the date
  http.useHTTP10(true); // HTTP1.1s chuncked transfer does not work when reading a byte stream
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  http.begin(request, root_ca);
  http.GET();
  Serial.println("Request successful");

  // JSON document
  DynamicJsonDocument doc(12288);
  DeserializationError error = deserializeJson(doc, http.getStream());

  if (error != error.Ok)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  JsonArray weather = doc["weather"];
  // For each hour of the day
  for (int i = 0; i <= 23; i++)
  {
    // Map the weather api response to the brightskyResponse struct, so array index equals hour
    JsonObject weather_i = weather[i];
    brightskyResponse.weather_item_icon[i] = weather_i["icon"];
    brightskyResponse.weather_item_temperature[i] = weather_i["temperature"];
    brightskyResponse.weather_item_precipitation[i] = weather_i["precipitation"];
    brightskyResponse.weather_item_wind_speed[i] = weather_i["wind_speed"];
    brightskyResponse.weather_item_sunshine[i] = weather_i["sunshine"];
    brightskyResponse.weather_item_cloud_cover[i] = weather_i["cloud_cover"];
  }
}

float dailyTotal(float values[], u_int8_t size)
{
  double total = 0.0;
  for (int i = 0; i <= size; i++)
  {
    if (values[i])
    {
      total = total + values[i];
    }
  }
  return (float)total;
}

int dailyTotal(int values[], u_int8_t size)
{
  int total = 0;
  for (int i = 0; i <= size; i++)
  {
    if (values[i])
    {
      total = total + values[i];
    }
  }
  return total;
}

float dailyAvg(float values[], u_int8_t size)
{
  return (dailyTotal(values, size) / size);
}

int dailyAvg(int values[], u_int8_t size)
{
  return (dailyTotal(values, size) / size);
}

float dailyMax(float values[], u_int8_t size)
{
  double max = values[0];
  for (int i = 0; i <= size; i++)
  {
    if (values[i] && values[i] > max)
    {
      max = values[i];
    }
  }
  return (float)max;
}

float dailyMin(float values[], u_int8_t size)
{
  double min = values[0];
  for (int i = 0; i <= size; i++)
  {
    if (values[i] && values[i] < min)
    {
      min = values[i];
    }
  }
  return (float)min;
}

void syncDWDForecast()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Getting the forecast depends on a WiFi connection!");
    return;
  }

  // For the forecast to work, we need to know what day it is today
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  Date today, tomorrow, overmorrow, overovermorrow;
  today.year = timeinfo.tm_year + 1900; // timeinfo.tm_year returns years since 1900
  today.month = timeinfo.tm_mon + 1;    // timeinfo.tm_mon returns months since January
  today.day = timeinfo.tm_mday;

  //Calculate the dates for the next 3 days
  getTomorrow(today, tomorrow);
  getTomorrow(tomorrow, overmorrow);
  getTomorrow(overmorrow, overovermorrow);

  // Get the forecast/current values for today
  Serial.println("Calling Brightsky-API for today");
  callBrightSkyApi(today);
  Serial.println("Call successful");

  // Map the last response to the values that will be displayed by the display
  Serial.println("Mapping API response");
  forecastData.todayIcon = iconTypeToMeteocon(brightskyResponse.weather_item_icon[timeinfo.tm_hour]); // Get the icon for the current hour
  forecastData.todayAvgTemp = dailyAvg(brightskyResponse.weather_item_temperature, 24);
  forecastData.todayMaxTemp = dailyMax(brightskyResponse.weather_item_temperature, 24);
  forecastData.todayMinTemp = dailyMin(brightskyResponse.weather_item_temperature, 24);
  forecastData.todayRainAmt = dailyMax(brightskyResponse.weather_item_temperature, 24);
  forecastData.todayWindSpeed = kmhToBft(brightskyResponse.weather_item_wind_speed[timeinfo.tm_hour]);
  int totalSunshine = dailyTotal(brightskyResponse.weather_item_sunshine, 24);
  forecastData.todaySunHrs = totalSunshine / 60;
  forecastData.todaySunMin = totalSunshine - (60 * forecastData.todaySunHrs);
  forecastData.todayOvercastPercent = dailyAvg(brightskyResponse.weather_item_cloud_cover, 24);

  // Get the forecast for tomorrow
  Serial.println("Calling Brightsky-API for tomorrow");
  callBrightSkyApi(tomorrow);
  forecastData.tomorrowIcon = iconTypeToMeteocon(brightskyResponse.weather_item_icon[timeinfo.tm_hour]);
  forecastData.tomorrowAvgTemp = dailyAvg(brightskyResponse.weather_item_temperature, 24);

  // Get the forecast for overmorrow
  Serial.println("Calling Brightsky-API for overmorrow");
  callBrightSkyApi(overmorrow);
  forecastData.overmorrowIcon = iconTypeToMeteocon(brightskyResponse.weather_item_icon[timeinfo.tm_hour]);
  forecastData.tomorrowAvgTemp = dailyAvg(brightskyResponse.weather_item_temperature, 24);

  // Get the forecast for overovermorrow
  Serial.println("Calling Brightsky-API for overovermorrow");
  callBrightSkyApi(overovermorrow);
  forecastData.overOvermorrowIcon = iconTypeToMeteocon(brightskyResponse.weather_item_icon[timeinfo.tm_hour]);
  forecastData.tomorrowAvgTemp = dailyAvg(brightskyResponse.weather_item_temperature, 24);
}

char getForecastTodayIcon() { return forecastData.todayIcon; }
float getForecastTodayAvgTemp() { return forecastData.todayAvgTemp; }
float getForecastTodayMaxTemp() { return forecastData.todayMaxTemp; }
float getForecastTodayMinTemp() { return forecastData.todayMinTemp; }
float getForecastTodayRainAmt() { return forecastData.todayRainAmt; }
float getForecastTodayWindSpeed() { return forecastData.todayWindSpeed; }
int getForecastTodaySunHrs() { return forecastData.todaySunHrs; }
int getForecastTodaySunMin() { return forecastData.todaySunMin; }
int getForecastTodayOvercastPercent() { return forecastData.todayOvercastPercent; }

char getForecastTomorrowIcon() { return forecastData.tomorrowIcon; }
float getForecastTomorrowAvgTemp() { return forecastData.tomorrowAvgTemp; }

char getForecastOvermorrowIcon() { return forecastData.overmorrowIcon; }
float getForecastOvermorrowAvgTemp() { return forecastData.overmorrowAvgTemp; }

char getForecastOverOvermorrowIcon() { return forecastData.overOvermorrowIcon; }
float getForecastOverOvermorrowAvgTemp() { return forecastData.overOvermorrowAvgTemp; }
