using namespace std;

#include <Arduino.h>

#include "services/WiFiService.h"
#include "services/WeekdayConverter.h"
#include "OutdoorSensor.h"
#include "Forecast.h"
#include "IndoorSensor.h"
#include "Timekeeping.h"

#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include "fonts/FiraMonoBold14pt7b.h"
#include "fonts/FiraMonoBold48pt7b.h"
#include "fonts/FiraMonoReg10pt7b.h"
#include "fonts/FiraMonoReg24pt7b.h"

#include "display_selection/GxEPD2_display_selection_new_style.h"

const int ONE_SECOND = 1000;

unsigned long previousMillisOutside = 0;
const long intervalOutside = (ONE_SECOND * 30); // DEBUG

unsigned long previousMillisInside = 0;
const long intervalInside = ONE_SECOND * 15;

unsigned long previousMillisTime = 0;
const long intervalTime = ONE_SECOND * 60;

unsigned long previousMillisWiFi = 0;
const long intervalWiFi = (ONE_SECOND * 60) * 60;

struct display_time
{
  char weekday[11];
  char day[3];
  char month[3];
  char year[5];
  char time[6];
};
display_time displayTime;

void drawGridCallback(const void *)
{
  display.fillScreen(GxEPD_WHITE);

  // Draw outer box (10 px)
  display.fillRect(0, 0, display.width(), 10, GxEPD_BLACK);
  display.fillRect(0, 10, 10, display.height(), GxEPD_BLACK);
  display.fillRect(display.width() - 10, 10, display.width(), display.height(), GxEPD_BLACK);
  display.fillRect(10, display.height() - 10, display.width(), display.height(), GxEPD_BLACK);

  // Draw horizontal divider
  display.fillRect(10, 410, display.width() - 10, 5, GxEPD_BLACK);

  // Draw vertical dividers
  display.fillRect(360, 10, 5, 400, GxEPD_BLACK);
  display.fillRect(835, 10, 5, 400, GxEPD_BLACK);

  // Print static text
  display.setCursor(20, 50);
  display.print("Draussen");

  display.setCursor(375, 50);
  display.print("Vorhersage");

  display.setCursor(850, 50);
  display.print("Drinnen");

  // Print some fun text, to let people know, that *something* is happening
  display.setCursor(240, (display.height() / 2) + (display.height() / 4));
  display.print("Ich fahre gerade hoch, einen Moment noch (X-X)");
}

void drawGrid()
{
  display.setFullWindow();
  display.setFont(&FiraMono_Bold14pt7b);
  display.drawPaged(drawGridCallback, 0);
}

void drawOutsideWeatherdataCallback(const void *)
{

  display.setFont(&FiraMono_Bold48pt7b);
  display.setCursor(48, 191);

  // Print temperature
  // If the outside temp is in negative double digits, we omit the decimal place
  if (getOutsideTemp() <= -10)
  {
    display.printf("%.0f", getOutsideTemp());
  }
  else
  {
    display.printf("%04.1f", getOutsideTemp());
  }

  // TODO: Print degree C symbol

  // Print rel. Humidity
  display.setFont(&FiraMono_Regular24pt7b);
  display.setCursor(52, 292);
  display.printf("%02.0f", getOutsideHumid());

  display.setFont(&FiraMono_Regular10pt7b);
  display.setCursor(120, 273);
  display.print("%");
  display.setCursor(114, 292);
  display.print("rh");

  // Print Pressure
  display.setFont(&FiraMono_Regular24pt7b);
  display.setCursor(167, 292);
  display.printf("%04.0f", getOutsidePress() / 100);

  display.setFont(&FiraMono_Regular10pt7b);
  display.setCursor(283, 292);
  display.print("hPa");
}

// callback function executed when data is received
void drawOutsideWeatherdata()
{
  display.setPartialWindow(20, 100, 322, 272);
  display.drawPaged(drawOutsideWeatherdataCallback, 0);
}

void drawForecastCallback(const void *)
{

  // Print todays temperatures
  // If the forecast avg temp is in negative double digits, we omit the decimal place
  display.setFont(&FiraMono_Regular24pt7b);
  display.setCursor(534, 155);
  if (getForecastTodayAvgTemp() <= -10)
  {
    display.printf("%.0f", getForecastTodayAvgTemp());
  }
  else
  {
    display.printf("%04.1f", getForecastTodayAvgTemp());
  }

  // Today max
  display.setFont(&FiraMono_Regular10pt7b);
  display.setCursor(534, 201);
  if (getForecastTodayMaxTemp() <= -10)
  {
    display.printf("%.0f", getForecastTodayMaxTemp());
  }
  else
  {
    display.printf("%04.1f", getForecastTodayMaxTemp());
  }

  // Spacing dot
  display.fillCircle(600, 195, 6, GxEPD_BLACK);
}

// callback function executed when data is received
void drawForecast()
{
  display.setPartialWindow(376, 72, 456, 323);
  display.drawPaged(drawForecastCallback, 0);
}

void drawInsideWeatherdataCallback(const void *)
{

  display.setFont(&FiraMono_Bold48pt7b);
  display.setCursor(878, 191);

  // Print temperature
  // Let's hope that the indoor temperature won't go into the negatives oAo; (hence no special formatting)
  display.printf("%04.1f", getIndoorTemp());

  // TODO: Print degree C symbol

  // Print rel. Humidity
  display.setFont(&FiraMono_Regular24pt7b);
  display.setCursor(884, 292);
  display.printf("%02.0f", getIndoorHumid());

  display.setFont(&FiraMono_Regular10pt7b);
  display.setCursor(947, 273);
  display.print("%");
  display.setCursor(941, 292);
  display.print("rh");

  // Print Pressure
  display.setFont(&FiraMono_Regular24pt7b);
  display.setCursor(994, 292);
  display.printf("%04.0f", getIndoorPress() / 100);

  display.setFont(&FiraMono_Regular10pt7b);
  display.setCursor(1111, 292);
  display.print("hPa");

  // Print AQI (with accuracy on top)
  const int AQI = getIndoorAQI(); // We need the value later for the smiley
  display.setFont(&FiraMono_Regular24pt7b);
  display.setCursor(916, 357);
  display.printf("%03d", AQI);

  display.setFont(&FiraMono_Regular10pt7b);
  display.setCursor(1002, 336);
  display.printf("(%d)", getIndoorAQIAccuracy());

  display.setFont(&FiraMono_Regular10pt7b);
  display.setCursor(1002, 357);
  display.print("AQI");

  // Print AQI-dependent smiley
  display.setFont(&FiraMono_Regular24pt7b);
  display.setCursor(1055, 357);

  // If the sensor is still stabilizing (acurracy = 0) and we therefore do not have a valid output, we print :X (and speak no evil)
  // See https://community.bosch-sensortec.com/t5/MEMS-sensors-forum/BME680-IAQ-accuracy-definition/td-p/5920
  if (getIndoorAQIAccuracy() == 0)
  {
    display.print(":X");
  }
  // Else we can orient ourselves on the AQI ratings from the BME680 datasheet (Table 4 in V1.7)
  else if (AQI < 50)
  {
    display.print(":D");
  }
  else if (AQI < 100)
  {
    display.print(":)");
  }
  else if (AQI < 150)
  {
    display.print(":/");
  }
  else if (AQI < 200)
  {
    display.print(":(");
  }
  else if (AQI < 250)
  {
    display.print(":C");
  }
  else
  {
    display.print("D:");
  }
}

// callback function executed when data is received
void drawInsideWeatherdata()
{
  display.setPartialWindow(850, 100, 322, 272);
  display.drawPaged(drawInsideWeatherdataCallback, 0);
}

void drawTimeAndDateCallback(const void *)
{
  display.setCursor(424, 555);
  display.fillScreen(GxEPD_WHITE);
  display.setFont(&FiraMono_Bold14pt7b);
  display.printf("%s, der %s.%s.%s", displayTime.weekday, displayTime.day, displayTime.month, displayTime.year);

  display.setCursor(467, 651);
  display.setFont(&FiraMono_Bold48pt7b);
  display.printf("%s", displayTime.time);
}

void drawTimeAndDate()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }

  strftime(displayTime.day, 3, "%d", &timeinfo);
  strftime(displayTime.month, 3, "%m", &timeinfo);
  strftime(displayTime.year, 5, "%Y", &timeinfo);
  strftime(displayTime.time, 6, "%R", &timeinfo);

  char weekdayInt[2];
  strftime(weekdayInt, 2, "%w", &timeinfo);

  strcpy(displayTime.weekday, intToGermanWeekday(weekdayInt[0]));

  // Lower Half of the Display
  display.setPartialWindow(24, (display.height() / 2) + 5, display.width() - 80, (display.height() / 2) - 20);
  display.drawPaged(drawTimeAndDateCallback, 0);
}

void setup()
{
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Display
  display.init(115200);
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(0);
  drawGrid();

  connectWiFi();
  syncTimeWithServer();
  Serial.println("NTP Setup done \n");

  syncDWDForecast();
  Serial.println("NTP Setup done \n");
  disconnectWiFi();

  initOutdoorSensor();
  Serial.println("ESP-Now Setup done \n");

  // initIndoorSensor();
  // Serial.println("Indoor-Sensor Setup done \n");

  Serial.println("Making first display update");
  drawOutsideWeatherdata();

  drawInsideWeatherdata();
  drawTimeAndDate();
}

void loop()
{

  unsigned long currentMillis = millis();

  // Update the clock
  if ((currentMillis - previousMillisTime) >= intervalTime)
  {
    previousMillisTime = currentMillis;
    drawTimeAndDate();

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
      Serial.println("Failed to obtain time");
      return;
    }

    // Make it so that the next update ocurrs 3s after the full minute, so we don't accidentally display the old minute
    // Race condition remover - A+ programming
    int seconds = timeinfo.tm_sec - 3;
    Serial.printf("Hasting the next time-update for %d seconds \n", (seconds));
    previousMillisTime -= seconds * 1000;
  }

  // Update the outdoor sensor values
  if ((currentMillis - previousMillisOutside) >= intervalOutside)
  {
    Serial.println("Updating Outside Temp");
    previousMillisOutside = currentMillis;
    drawOutsideWeatherdata();
  }

  // Update the indoor sensor values
  if ((currentMillis - previousMillisInside) >= intervalInside)
  {
    Serial.println("Updating Inside Temp");
    previousMillisInside = currentMillis;
    drawInsideWeatherdata();
  }

  // loopIndoorSensor();

  delay(500);
}

// DEBUGGERINO
float getIndoorTemp() { return 22.2; }
float getIndoorPress() { return 102576.80; }
float getIndoorHumid() { return 67.55; }
int getIndoorAQI() { return 25; }
int getIndoorAQIAccuracy() { return 3; }
