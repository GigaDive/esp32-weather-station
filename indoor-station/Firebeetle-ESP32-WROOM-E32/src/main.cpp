using namespace std;

#include <Arduino.h>

#include "OutdoorSensor.h"
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

unsigned long previousMillisTime = 0;
const long intervalTime = ONE_SECOND * 60;

unsigned long previousMillisOutside = 0;
const long intervalOutside = (ONE_SECOND * 30); // DEBUG

unsigned long previousMillisInside = 0;
const long intervalInside = ONE_SECOND * 60;

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
  display.setCursor(52, 292);
  display.setFont(&FiraMono_Regular24pt7b);
  display.printf("%02.0f", getOutsideHumid());

  display.setFont(&FiraMono_Regular10pt7b);
  display.setCursor(120, 273);
  display.print("%");
  display.setCursor(114, 292);
  display.print("rh");

  // Print Pressure
  display.setCursor(167, 292);
  display.setFont(&FiraMono_Regular24pt7b);
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

  switch (weekdayInt[0])
  {
  case '0':
    strcpy(displayTime.weekday, "Sonntag");
    break;
  case '1':
    strcpy(displayTime.weekday, "Montag");
    break;
  case '2':
    strcpy(displayTime.weekday, "Dienstag");
    break;
  case '3':
    strcpy(displayTime.weekday, "Mittwoch");
    break;
  case '4':
    strcpy(displayTime.weekday, "Donnerstag");
    break;
  case '5':
    strcpy(displayTime.weekday, "Freitag");
    break;
  case '6':
    strcpy(displayTime.weekday, "Samstag");
    break;

  default:
    strcpy(displayTime.weekday, "Fehler");
    break;
  }

  // Lower Half of the Display
  display.setPartialWindow(15, (display.height() / 2) + 5, display.width() - 40, (display.height() / 2) - 20);
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

  // initIndoorSensor();
  // Serial.println("Indoor-Sensor Setup done \n");

  syncTimeWithServer();
  Serial.println("NTP Setup done \n");

  initOutdoorSensor();
  Serial.println("ESP-Now Setup done \n");

  Serial.println("Making first display update");
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

  // loopIndoorSensor();

  delay(500);
}
