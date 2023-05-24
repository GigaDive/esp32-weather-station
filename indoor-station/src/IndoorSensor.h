#ifndef INDOOR_SENSOR
#define INDOOR_SENSOR

void initIndoorSensor();

void loopIndoorSensor();

float getIndoorTemp();

float getIndoorPress();

float getIndoorHumid();

int getIndoorAQI();

int getIndoorAQIAccuracy();

#endif