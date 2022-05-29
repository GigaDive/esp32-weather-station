#ifndef INDOOR_SENSOR
#define OUTDOOR_SENSOR

void initIndoorSensor();
void loopIndoorSensor();

float getIndoorTemp();
float getIndoorPress();
float getIndoorHumid();
float getIndoorAQI();

#endif