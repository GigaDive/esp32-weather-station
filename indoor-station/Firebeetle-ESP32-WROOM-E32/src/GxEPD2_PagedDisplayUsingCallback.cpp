// Display Library example for SPI e-paper panels from Dalian Good Display and boards from Waveshare.
// Requires HW SPI and Adafruit_GFX. Caution: the e-paper panels require 3.3V supply AND data lines!
//
// Display Library based on Demo Example from Good Display: http://www.e-paper-display.com/download_list/downloadcategoryid=34&isMode=false.html
//
// Author: Jean-Marc Zingg
//
// Version: see library.properties
//
// Library: https://github.com/ZinggJM/GxEPD2

// Supporting Arduino Forum Topics:
// Waveshare e-paper displays with SPI: http://forum.arduino.cc/index.php?topic=487007.0
// Good Display ePaper for Arduino: https://forum.arduino.cc/index.php?topic=436411.0

// see GxEPD2_wiring_examples.h for wiring suggestions and examples

// base class GxEPD2_GFX can be used to pass references or pointers to the display instance as parameter, uses ~1.2k more code
// enable or disable GxEPD2_GFX base class
#define ENABLE_GxEPD2_GFX 0

// uncomment next line to use class GFX of library GFX_Root instead of Adafruit_GFX
//#include <GFX.h>
// Note: if you use this with ENABLE_GxEPD2_GFX 1:
//       uncomment it in GxEPD2_GFX.h too, or add #include <GFX.h> before any #include <GxEPD2_GFX.h>

#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include "fonts/FiraMonoBold24pt7b.h"
#include "fonts/FiraMonoBold36pt7b.h"
#include "fonts/FiraMonoReg20pt7b.h"
#include "fonts/FiraMonoReg48pt7b.h"

// select the display class and display driver class in the following file (new style):
#include "display_selection/GxEPD2_display_selection_new_style.h"


void helloWorldCallback(const void*)
{
  uint16_t x = (display.width() - 160) / 2;
  uint16_t y = display.height() / 2;
  display.fillScreen(GxEPD_WHITE);
  display.setCursor(x, y);
  display.println("Hello World!");
}

void helloWorld()
{
  //Serial.println("helloWorld");
  uint16_t x = (display.width() - 160) / 2;
  uint16_t y = display.height() / 2;
  display.setRotation(0);
  display.setFont(&FiraMono_Bold24pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setFullWindow();
  display.drawPaged(helloWorldCallback, 0);
  //Serial.println("helloWorld done");
}

void helloFullScreenPartialModeCallback(const void*)
{
  uint16_t x = (display.width() - 160) / 2;
  uint16_t y = display.height() / 2;
  display.fillScreen(GxEPD_WHITE);
  display.setCursor(x, y);
  display.println("Hello World!");
  y = display.height() / 4;
  display.setCursor(x, y);
  display.println("full screen");
  y = display.height() * 3 / 4;
  if (display.width() <= 200) x = 0;
  display.setCursor(x, y);
  if (display.epd2.hasFastPartialUpdate)
  {
    display.println("fast partial mode");
  }
  else if (display.epd2.hasPartialUpdate)
  {
    display.println("slow partial mode");
  }
  else
  {
    display.println("no partial mode");
  }
}

void helloFullScreenPartialMode()
{
  //Serial.println("helloFullScreenPartialMode");
  display.setPartialWindow(0, 0, display.width(), display.height());
  display.setRotation(0);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.drawPaged(helloFullScreenPartialModeCallback, 0);
  //Serial.println("helloFullScreenPartialMode done");
}

void helloArduinoCallback(const void*)
{
  uint16_t x = (display.width() - 160) / 2;
  uint16_t y = display.height() / 4;
  display.fillScreen(GxEPD_WHITE);
  display.setCursor(x, y);
  display.println("Hello Arduino!");
}

void helloArduino()
{
  //Serial.println("helloArduino");
  uint16_t x = (display.width() - 160) / 2;
  uint16_t y = display.height() / 4;
  display.setRotation(0);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(display.epd2.hasColor ? GxEPD_RED : GxEPD_BLACK);
  display.setPartialWindow(0, y - 14, display.width(), 20);
  display.drawPaged(helloArduinoCallback, 0);
  delay(1000);
  //Serial.println("helloArduino done");
}

void helloEpaperCallback(const void*)
{
  uint16_t x = (display.width() - 160) / 2;
  uint16_t y = display.height() * 3 / 4;
  display.fillScreen(GxEPD_WHITE);
  display.setCursor(x, y);
  display.println("Hello E-Paper!");
}

void helloEpaper()
{
  //Serial.println("helloEpaper");
  uint16_t x = (display.width() - 160) / 2;
  uint16_t y = display.height() * 3 / 4;
  display.setRotation(0);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(display.epd2.hasColor ? GxEPD_RED : GxEPD_BLACK);
  display.setPartialWindow(0, y - 14, display.width(), 20);
  display.drawPaged(helloEpaperCallback, 0);
  //Serial.println("helloEpaper done");
}

struct DrawFontParameters
{
  const char* name;
  const GFXfont* f;
};

void drawFontCallback(const void* parameters)
{
  const DrawFontParameters* p = reinterpret_cast<const DrawFontParameters*>(parameters);
  //display.setRotation(0);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(p->f);
  display.setCursor(0, 0);
  display.println();
  display.println(p->name);
  display.println(" !\"#$%&'()*+,-./");
  display.println("0123456789:;<=>?");
  display.println("@ABCDEFGHIJKLMNO");
  display.println("PQRSTUVWXYZ[\\]^_");
  if (display.epd2.hasColor)
  {
    display.setTextColor(GxEPD_RED);
  }
  display.println("`abcdefghijklmno");
  display.println("pqrstuvwxyz{|}~ ");
}

void showFont(const char name[], const GFXfont* f)
{
  DrawFontParameters fontParameters{name, f};
  display.setFullWindow();
  display.setRotation(0);
  display.setTextColor(GxEPD_BLACK);
  display.drawPaged(drawFontCallback, &fontParameters);
}


struct ShowBoxParameters
{
  uint16_t x, y, w, h;
  uint16_t color;
};

struct ShowValueParameters
{
  uint16_t x, y, w, h, cursor_y;
  float value;
};

void showBoxCallback(const void* parameters)
{
  const ShowBoxParameters* p = reinterpret_cast<const ShowBoxParameters*>(parameters);
  display.fillRect(p->x, p->y, p->w, p->h, p->color);
}

void showValueBoxCallback(const void* parameters)
{
  const ShowValueParameters* p = reinterpret_cast<const ShowValueParameters*>(parameters);
  display.fillRect(p->x, p->y, p->w, p->h, GxEPD_WHITE);
  display.setCursor(p->x, p->cursor_y);
  display.print(p->value);
}

void showPartialUpdate()
{
  Serial.println("Partial Updates");
  // some useful background
  helloWorld();
  // use asymmetric values for test
  ShowBoxParameters boxParameters{10, 15, 70, 20, GxEPD_WHITE};
  ShowValueParameters valueParameters{10, 15, 70, 20, 14};
  valueParameters.cursor_y = valueParameters.y + valueParameters.h - 6;
  float value = 13.95;
  uint16_t incr = display.epd2.hasFastPartialUpdate ? 1 : 3;
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  // show where the update box is
  for (uint16_t r = 0; r < 4; r++)
  {
    display.setRotation(r);
    display.setPartialWindow(boxParameters.x, boxParameters.y, boxParameters.w, boxParameters.h);
    boxParameters.color = GxEPD_BLACK;
    display.drawPaged(showBoxCallback, &boxParameters);
    delay(2000);
    boxParameters.color = GxEPD_WHITE;
    display.drawPaged(showBoxCallback, &boxParameters);
    while (display.nextPage());
    delay(1000);
  }
  // show updates in the update box
  for (uint16_t r = 0; r < 4; r++)
  {
    display.setRotation(r);
    display.setPartialWindow(boxParameters.x, boxParameters.y, boxParameters.w, boxParameters.h);
    for (uint16_t i = 1; i <= 10; i += incr)
    {
      valueParameters.value = value * i;
      display.drawPaged(showValueBoxCallback, &valueParameters);
      delay(500);
    }
    delay(1000);
    boxParameters.color = GxEPD_WHITE;
    display.drawPaged(showBoxCallback, &boxParameters);
    delay(1000);
  }
}

void showBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool partial)
{
  //Serial.println("showBox");
  ShowBoxParameters boxParameters{x, y, w, h, GxEPD_BLACK};
  display.setRotation(0);
  if (partial)
  {
    display.setPartialWindow(x, y, w, h);
  }
  else
  {
    display.setFullWindow();
  }
  display.drawPaged(showBoxCallback, &boxParameters);
  //Serial.println("showBox done");
}

void drawCornerTestCallback(const void*)
{
  display.fillScreen(GxEPD_WHITE);
  display.fillRect(0, 0, 8, 8, GxEPD_BLACK);
  display.fillRect(display.width() - 18, 0, 16, 16, GxEPD_BLACK);
  display.fillRect(display.width() - 25, display.height() - 25, 24, 24, GxEPD_BLACK);
  display.fillRect(0, display.height() - 33, 32, 32, GxEPD_BLACK);
  display.setCursor(display.width() / 2, display.height() / 2);
  display.print(display.getRotation());
}

void drawCornerTest()
{
  display.setFullWindow();
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  for (uint16_t r = 0; r <= 4; r++)
  {
    display.setRotation(r);
    display.drawPaged(drawCornerTestCallback, 0);
    delay(2000);
  }
}

void setup_paged()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
  display.init(115200);
  // first update should be full refresh
  Serial.println("helloWorld");
  helloWorld();
  delay(1000);
  // partial refresh mode can be used to full screen,
  // effective if display panel hasFastPartialUpdate
  Serial.println("helloFullScreenPartialMode");
  helloFullScreenPartialMode();
  delay(1000);
  Serial.println("helloArduino");
  helloArduino();
  delay(1000);
  Serial.println("helloEpaper");
  helloEpaper();
  delay(1000);
  Serial.println("showFont");
  showFont("FiraMono_Bold24pt7b", &FiraMono_Bold24pt7b);
  delay(1000);
  showFont("FiraMono_Bold36pt7b", &FiraMono_Bold36pt7b);
  delay(1000);
  showFont("FiraMono_Reg20pt7b", &FiraMono_Regular20pt7b);
  delay(1000);
  showFont("FiraMono_Reg48pt7b", &FiraMono_Regular48pt7b);
  delay(1000);

  if (display.epd2.hasPartialUpdate)
  {
    showPartialUpdate();
    delay(1000);
  } // else // on GDEW0154Z04 only full update available, doesn't look nice
  //drawCornerTest();
  //showBox(16, 16, 48, 32, false);
  //showBox(16, 56, 48, 32, true);
  display.powerOff();
  Serial.println("setup done");
}