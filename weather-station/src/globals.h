#include <Arduino.h>
#ifndef GLOBALS
#define GLOBALS

extern String DEVICE_ID = "WStation";
extern String MQTT_SERVER = "MQTT_SERVER";
extern int MQTT_PORT = 1880;

#ifdef DEVICE_ID
#pragma message STR(DEVICE_ID)
#endif
#ifdef MQTT_SERVER
#pragma message STR(MQTT_SERVER)
#endif
#ifdef MQTT_PORT
#pragma message STR(MQTT_PORT)
#endif
#endif
