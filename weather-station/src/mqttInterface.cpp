#include "Arduino.h"
#include "mqttInterface.h"
#include "globals.h"
#include <PubSubClient.h>
#include <WiFi.h>

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

String MQTT_CHANNEL_OUTDOOR_TEMPERATURE = "/sensor/weather/" + DEVICE_ID + "/TEMPERATURE";
String MQTT_CHANNEL_OUTDOOR_MOISTURE = "/sensor/weather/" + DEVICE_ID + "/MOISTURE";
String MQTT_CHANNEL_OUTDOOR_AIR_PRESSURE = "/sensor/weather/" + DEVICE_ID + "/AIR_PRESSURE";
long millisPublish = millis();

void callback(char *topic, byte *message, unsigned int length)
{
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String messageTemp;

    for (int i = 0; i < length; i++)
    {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
    Serial.println();

    if (MQTT_CHANNEL_OUTDOOR_TEMPERATURE == topic)
    {
        Serial.println("TEMP DATA RECEIVED");
    }
    if (MQTT_CHANNEL_OUTDOOR_MOISTURE == topic)
    {
        Serial.println("MOISTURE DATA RECEIVED");
    }
    if (MQTT_CHANNEL_OUTDOOR_AIR_PRESSURE == topic)
    {
        Serial.println("OUTDOOR AIR PRESSURE DATA RECEIVED");
    }
}

void setupMQTT()
{
    Serial.print("SETUP MQTT-SERVER: ");
    Serial.println(MQTT_SERVER);
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(callback);
    //mqttClient.unsubscribe("/channel/");
}

bool mqttConnect()
{
    uint8_t retry = 5;
    while (!mqttClient.connected())
    {
        if (DEVICE_ID.length() <= 0)
            break;
        Serial.println(String("Attempting MQTT broker:") + MQTT_SERVER);
        if (mqttClient.connect(DEVICE_ID, "testuser", "testpass"))
        {
            mqttClient.subscribe(MQTT_CHANNEL_OUTDOOR_TEMPERATURE);
            mqttClient.subscribe(MQTT_CHANNEL_OUTDOOR_MOISTURE);
            mqttClient.subscribe(MQTT_CHANNEL_OUTDOOR_AIR_PRESSURE);

            Serial.println("Established:" + String(DEVICE_ID));
            return true;
        }
        else
        {
            Serial.println("Connection failed:" + String(mqttClient.state()));
            if (!--retry)
                break;
            delay(3000);
        }
    }
    return false;
}

void loopMQTT()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        // MQTT publish control
        if (millis() - millisPublish >= 1000)
        {
            if (!mqttClient.connected())
            {
                mqttConnect();
            }
            millisPublish = millis();
        }
    }
    mqttClient.loop();
}