// uncomment next line to use class GFX of library GFX_Root instead of Adafruit_GFX, to use less code and ram
// #include <GFX.h>

// #include <GxEPD2_BW.h> // including both doesn't use more code or ram

// select the display class and display driver class in the following file (new style):
// #include "display_selection/GxEPD2_display_selection_new_style.h"

// alternately you can copy the constructor from GxEPD2_display_selection.h or GxEPD2_display_selection_added.h of GxEPD2_Example to here
// GxEPD2_BW<GxEPD2_it60_1200x825, GxEPD2_it60_1200x825::HEIGHT / 4> display(GxEPD2_it60_1200x825(/*CS*/ 12, /*DC*/23, /*RST*/16, /*BUSY*/17));
/*
void setup_example()
{
    Serial.println("Init display :3");
    display.init();
    // comment out next line to have no or minimal Adafruit_GFX code

    display.setTextColor(GxEPD_BLACK);
    display.firstPage();
    Serial.println("Write sumthing ....UwU :3");

    display.fillScreen(GxEPD_WHITE);
    display.setCursor(100, 100);
    display.print("Hello World!");
    display.fillRect(400, 400, 200, 200, GxEPD_BLACK);
    display.print("Moar Text!!11!11!11!11!132135221312321232");
    display.nextPage();

    delay(1000);
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(100, 100);
    display.print("Now the text is different!");
    display.fillRect(200, 200, 200, 200, GxEPD_BLACK);
    display.firstPage();
}

// void loop(){};

*/