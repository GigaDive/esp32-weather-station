#ifndef OUTDOOR_SENSOR
#define OUTDOOR_SENSOR

void initOutdoorSensor();
void deinitOutdoorSensor();

float getOutsideTemp();
float getOutsidePress();
float getOutsideHumid();
#endif