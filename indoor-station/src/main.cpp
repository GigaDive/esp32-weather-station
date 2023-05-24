using namespace std;

#include <Arduino.h>

#include "services/WiFiService.h"
#include "services/WeekdayConverter.h"
#include "services/IconMapper.h"
#include "OutdoorSensor.h"
#include "Forecast.h"
#include "IndoorSensor.h"
#include "Timekeeping.h"

#define ENABLE_GxEPD2_GFX 0
#define uS_TO_S_FACTOR 1000000 // Conversion factor for micro seconds to seconds

#include <GxEPD2_BW.h>
#include "fonts/FiraMonoBold14pt7b.h"
#include "fonts/FiraMonoBold48pt7b.h"
#include "fonts/FiraMonoReg10pt7b.h"
#include "fonts/FiraMonoReg24pt7b.h"

#include "fonts/Meteocons48pt7b.h"
#include "fonts/Meteocons70pt7b.h"

#include "fonts/Meteocons20pt7b.h"
#include "fonts/Meteocons12pt7b.h"

#include "display_selection/GxEPD2_display_selection_new_style.h"

const int ONE_SECOND = 1000;

unsigned long previousMillisOutside = 0;
const long intervalOutside = (ONE_SECOND * 60) * 5;

unsigned long previousMillisInside = 0;
const long intervalInside = ONE_SECOND * 30;

unsigned long previousMillisTime = 0;
const long intervalTime = ONE_SECOND * 60;

unsigned long previousMillisForecast = 0;
const long intervalForecast = (ONE_SECOND * 60) * 30;

unsigned long previousMillisDisplayReset = 0;
const long intervalDisplayReset = (ONE_SECOND * 60) * 60 * 24;

struct display_time {
    char weekday[11];
    char day[3];
    char month[3];
    char year[5];
    char time[6];
};
display_time displayTime;

struct tm timeinfo;

void drawGridCallback(const void *) {
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

    // Print some fun text, to let people know that *something* is happening
    display.setCursor(240, (display.height() / 2) + (display.height() / 4));
    display.print("Ich fahre gerade hoch, einen Moment noch (X-X)");
}

void refreshCallbackBLK(const void *) {
    display.fillScreen(GxEPD_BLACK);
}

void refreshCallbackWT(const void *) {
    display.fillScreen(GxEPD_WHITE);
}

void drawReset() {
    display.setFullWindow();
    display.drawPaged(refreshCallbackBLK, 0);
    display.drawPaged(refreshCallbackWT, 0);
}

void drawGrid() {
    display.setFullWindow();
    display.setFont(&FiraMono_Bold14pt7b);
    display.drawPaged(drawGridCallback, 0);
}

void drawOutsideWeatherdataCallback(const void *) {
    display.setFont(&FiraMono_Bold48pt7b);
    display.setCursor(48, 191);

    // Print temperature
    // If the outside temp is in negative double digits, we omit the decimal place
    if (getOutsideTemp() <= -10) {
        display.printf("%.0f", getOutsideTemp());
    } else {
        display.printf("%04.1f", getOutsideTemp());
    }

    // Print degree C symbol
    display.setFont(&meteocons48pt7b);
    display.setCursor(251, 183);
    display.print("*");

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

void drawOutsideWeatherdata() {
    display.setPartialWindow(20, 100, 322, 272);
    display.drawPaged(drawOutsideWeatherdataCallback, 0);
}

void drawForecastCallback(const void *) {

    // Print today's icon
    display.setFont(&meteocons70pt7b);
    display.setCursor(388, 200);
    display.print(getForecastTodayIcon());

    // Print today's temperatures
    // If the forecast avg temp is in negative double digits, we omit the decimal place
    display.setFont(&FiraMono_Regular24pt7b);
    display.setCursor(534, 155);
    if (getForecastTodayAvgTemp() <= -10) {
        display.printf("%.0f", getForecastTodayAvgTemp());
    } else {
        display.printf("%04.1f", getForecastTodayAvgTemp());
    }
    display.setFont(&meteocons20pt7b);
    display.setCursor(638, 147);
    display.print("*");

    // Today max
    display.setFont(&FiraMono_Regular10pt7b);
    display.setCursor(534, 201);
    if (getForecastTodayMaxTemp() <= -10) {
        display.printf("%.0f", getForecastTodayMaxTemp());
    } else {
        display.printf("%04.1f", getForecastTodayMaxTemp());
    }

    // Spacing dot
    display.fillCircle(600, 195, 3, GxEPD_BLACK);

    // Today min
    display.setFont(&FiraMono_Regular10pt7b);
    display.setCursor(610, 201);
    if (getForecastTodayMinTemp() <= -10) {
        display.printf("%.0f", getForecastTodayMinTemp());
    } else {
        display.printf("%04.1f", getForecastTodayMinTemp());
    }

    // Rain Amount
    display.setFont(&meteocons12pt7b);
    display.setCursor(693, 107);
    display.print("8"); // Raincloud
    display.setFont(&FiraMono_Regular10pt7b);
    display.setCursor(723, 101);
    display.printf("%3.1f mm/h", getForecastTodayRainAmt());

    // Wind speed
    display.setFont(&meteocons12pt7b);
    display.setCursor(693, 141);
    display.print("F"); // Wind
    display.setFont(&FiraMono_Regular10pt7b);
    display.setCursor(723, 135);
    display.printf("%-.0f bft", getForecastTodayWindSpeed());

    // Sun-hours
    display.setFont(&meteocons12pt7b);
    display.setCursor(693, 175);
    display.print("1"); // Sun
    display.setFont(&FiraMono_Regular10pt7b);
    display.setCursor(723, 170);
    display.printf("%d:%02d h", getForecastTodaySunHrs(), getForecastTodaySunMin());

    // Overcast percentage
    display.setFont(&meteocons12pt7b);
    display.setCursor(692, 208);
    display.print("5"); // Cloud
    display.setFont(&FiraMono_Regular10pt7b);
    display.setCursor(723, 203);
    display.printf("%-d %%", getForecastTodayOvercastPercent());

    // For the date above the forecast, we need to know what day it is today

    // Tomorrow day
    display.setFont(&FiraMono_Regular10pt7b);
    display.setCursor(416, 274);
    display.printf("%s", intToGermanWeekday(timeinfo.tm_wday + 1));
    // Tomorrow Icon
    display.setFont(&meteocons48pt7b);
    display.setCursor(411, 364);
    display.print(getForecastTomorrowIcon());
    // Tomorrow Temp
    display.setFont(&FiraMono_Regular10pt7b);
    display.setCursor(429, 389);
    display.printf("%.0f", getForecastTomorrowTemp());
    display.setFont(&meteocons20pt7b);
    display.setCursor(455, 399);
    display.print("*");

    // Overmorrow day
    display.setFont(&FiraMono_Regular10pt7b);
    display.setCursor(555, 274);
    display.printf("%s", intToGermanWeekday(timeinfo.tm_wday + 2));
    // Overmorrow Icon
    display.setFont(&meteocons48pt7b);
    display.setCursor(553, 364);
    display.print(getForecastOvermorrowIcon());
    // Overmorrow Temp
    display.setFont(&FiraMono_Regular10pt7b);
    display.setCursor(571, 389);
    display.printf("%.0f", getForecastOvermorrowTemp());
    display.setFont(&meteocons20pt7b);
    display.setCursor(597, 399);
    display.print("*");

    // Overovermorrow day (the day after the day after tomorrow)
    display.setFont(&FiraMono_Regular10pt7b);
    display.setCursor(699, 274);
    display.printf("%s", intToGermanWeekday(timeinfo.tm_wday + 3));
    // Overovermorrow Icon
    display.setFont(&meteocons48pt7b);
    display.setCursor(694, 364);
    display.print(getForecastOvermorrowIcon());
    // Overovermorrow Temp
    display.setFont(&FiraMono_Regular10pt7b);
    display.setCursor(712, 389);
    display.printf("%.0f", getForecastOvermorrowTemp());
    display.setFont(&meteocons20pt7b);
    display.setCursor(738, 399);
    display.print("*");
}

void drawForecast() {
    display.setPartialWindow(376, 72, 456, 323);
    display.drawPaged(drawForecastCallback, 0);
}

void drawInsideWeatherdataCallback(const void *) {

    display.setFont(&FiraMono_Bold48pt7b);
    display.setCursor(878, 191);

    // Print temperature.
    // Let's hope that the indoor temperature won't go into the negatives oAo; (hence no special formatting)
    display.printf("%04.1f", getIndoorTemp());

    // Print degree C symbol
    display.setFont(&meteocons48pt7b);
    display.setCursor(1081, 183);
    display.print("*");

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

    // If the sensor is still stabilizing (accuracy = 0) and we therefore do not have a valid output, we print :X (and speak no evil)
    // See https://community.bosch-sensortec.com/t5/MEMS-sensors-forum/BME680-IAQ-accuracy-definition/td-p/5920
    if (getIndoorAQIAccuracy() == 0) {
        display.print(":X");
    }
        // Else we can orient ourselves on the AQI ratings from the BME680 datasheet (Table 4 in V1.7)
    else if (AQI < 50) {
        display.print(":D");
    } else if (AQI < 100) {
        display.print(":)");
    } else if (AQI < 150) {
        display.print(":/");
    } else if (AQI < 200) {
        display.print(":(");
    } else if (AQI < 250) {
        display.print(":C");
    } else {
        display.print("D:");
    }
}

void drawInsideWeatherdata() {
    display.setPartialWindow(850, 100, 322, 272);
    display.drawPaged(drawInsideWeatherdataCallback, 0);
}

void drawTimeAndDateCallback(const void *) {
    display.setCursor(424, 555);
    display.fillScreen(GxEPD_WHITE);
    display.setFont(&FiraMono_Bold14pt7b);
    display.printf("%s, der %s.%s.%s", displayTime.weekday, displayTime.day, displayTime.month, displayTime.year);

    display.setCursor(467, 651);
    display.setFont(&FiraMono_Bold48pt7b);
    display.printf("%s", displayTime.time);
}

void drawTimeAndDate() {
    strftime(displayTime.day, 3, "%d", &timeinfo);
    strftime(displayTime.month, 3, "%m", &timeinfo);
    strftime(displayTime.year, 5, "%Y", &timeinfo);
    strftime(displayTime.time, 6, "%R", &timeinfo);

    strcpy(displayTime.weekday, intToGermanWeekday(timeinfo.tm_wday));

    // Lower Half of the Display
    display.setPartialWindow(24, (display.height() / 2) + 5, display.width() - 80, (display.height() / 2) - 20);
    display.drawPaged(drawTimeAndDateCallback, 0);
}

void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);

    // Display
    display.init(115200);
    display.setTextColor(GxEPD_BLACK);
    display.setRotation(0);
    drawReset();
    drawGrid();

    // Try to connect the Wi-Fi
    if (connectWiFi()) {
        syncTimeWithServer();
        // Make first update to time struct
        if (!getLocalTime(&timeinfo)) {
            Serial.println("Failed to obtain time");
            return;
        }
        Serial.println("NTP Setup done \n");

        syncDWDForecast();
        Serial.println("DWD Setup done \n");
    } else {
        // If Wi-Fi didn't work, we turn the whole thing off and on again
        esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
        esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
        esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
        esp_sleep_enable_timer_wakeup(10 * uS_TO_S_FACTOR);
        esp_deep_sleep_start();
    }
    disconnectWiFi();

    initESPNow();
    Serial.println("ESP-Now Setup done \n");

    initIndoorSensor();
    Serial.println("Indoor-Sensor Setup done \n");

    Serial.println("Making first display update");
    loopIndoorSensor();
    drawOutsideWeatherdata();
    drawForecast();
    drawInsideWeatherdata();
    drawTimeAndDate();
}

void loop() {
    unsigned long currentMillis = millis();

    // Update the clock
    if ((currentMillis - previousMillisTime) >= intervalTime) {
        previousMillisTime = currentMillis;

        if (!getLocalTime(&timeinfo)) {
            Serial.println("Failed to obtain time");
            return;
        }

        drawTimeAndDate();

        // Make it so that the next update occurs 3s after the full minute, so we don't accidentally display the old minute
        // Race condition remover - A+ programming
        int seconds = timeinfo.tm_sec - 3;
        Serial.printf("Hasting the next time-update for %d seconds \n", (seconds));
        previousMillisTime -= seconds * 1000;
        display.powerOff();
        Serial.println();
    }

    // Update the outdoor sensor values
    if ((currentMillis - previousMillisOutside) >= intervalOutside) {
        Serial.println("Updating Outside Temp");
        previousMillisOutside = currentMillis;
        drawOutsideWeatherdata();
        display.powerOff();
        Serial.println();
    }

    // Poll the DWD/Brightsky API as well as the NTP server and update the forecast values
    if ((currentMillis - previousMillisForecast) >= intervalForecast) {
        Serial.println("Updating Forecast data");
        previousMillisForecast = currentMillis;

        // Disable ESPNow and enable Wi-Fi
        deinitESPNow();
        if (connectWiFi()) {
            // If enabling the Wi-Fi worked, sync up with the DWD and NTP servers
            syncDWDForecast();
            syncTimeWithServer();
        }
        // Disable WiFi and enable ESPNow again
        disconnectWiFi();
        initESPNow();

        drawForecast();
        display.powerOff();
        Serial.println();
    }

    // Update the indoor sensor values
    if ((currentMillis - previousMillisInside) >= intervalInside) {
        Serial.println("Updating Inside Temp");
        previousMillisInside = currentMillis;
        drawInsideWeatherdata();
        display.powerOff();
        Serial.println();
    }

    // Reset the screen to prevent burn-in
    if ((currentMillis - previousMillisDisplayReset) >= intervalDisplayReset) {
        Serial.println("Resetting the screen");
        previousMillisDisplayReset = currentMillis;
        drawReset();
        drawGrid();
        drawOutsideWeatherdata();
        drawForecast();
        drawInsideWeatherdata();
        drawTimeAndDate();
        display.powerOff();
        Serial.println();
    }

    loopIndoorSensor();

    delay(500);
}