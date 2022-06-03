#ifndef OUTDOOR_SENSOR
#define OUTDOOR_SENSOR

void initESPNow();
void deinitESPNow();

float getOutsideTemp();
float getOutsidePress();
float getOutsideHumid();
#endif