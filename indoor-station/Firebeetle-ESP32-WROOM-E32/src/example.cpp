// GxEPD2_MinimumExample.ino by Jean-Marc Zingg

// purpose is e.g. to determine minimum code and ram use by this library

// see GxEPD2_wiring_examples.h of GxEPD2_Example for wiring suggestions and examples
// if you use a different wiring, you need to adapt the constructor parameters!

// uncomment next line to use class GFX of library GFX_Root instead of Adafruit_GFX, to use less code and ram
//#include <GFX.h>

#include <GxEPD2_BW.h> // including both doesn't use more code or ram
#include <GxEPD2_3C.h> // including both doesn't use more code or ram

// select the display class and display driver class in the following file (new style):
#include "GxEPD2_display_selection_new_style.h"

// alternately you can copy the constructor from GxEPD2_display_selection.h or GxEPD2_display_selection_added.h of GxEPD2_Example to here
// e.g. for Wemos D1 mini:
// GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(/*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4)); // GDEH0154D67
GxEPD2_BW<GxEPD2_it60_1448x1072, GxEPD2_it60_1448x1072::HEIGHT / 8> display(GxEPD2_it60_1448x1072(/*CS=5*/ 12, /*DC=*/23, /*RST=*/16, /*BUSY=*/17));
void setup_example()
{
    
    Serial.println("Init display :3");
    display.init();
    // comment out next line to have no or minimal Adafruit_GFX code
    
    display.setTextColor(GxEPD_BLACK);
    display.firstPage();
    Serial.println("Write sumthing ....UwU :3");

    do
    {
        display.fillScreen(GxEPD_WHITE);
        display.fillScreen(GxEPD_BLACK);
        display.fillScreen(GxEPD_WHITE);
        display.setCursor(100, 100);
        display.print("Hello World!");
        display.fillRect(400, 400, 200, 200, GxEPD_BLACK);
        // comment out next line to have no or minimal Adafruit_GFX code
        display.print("Moar Text!!11!11!11!11!132135221312321232");
        Serial.println("Hemlo World <3");

    } while (display.nextPage());
}

//void loop(){};