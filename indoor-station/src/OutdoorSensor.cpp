using namespace std;

#include "OutdoorSensor.h"
#include "services/Secrets.h"

#include <Arduino.h>

#include <esp_now.h>
#include <WiFi.h>

struct temp_humidity_pressure {
    float temperature;
    float humidity;
    float pressure;
};
temp_humidity_pressure packet;


// Set the SENDER MAC Address
uint8_t masterAddress[] = SENDER_MAC;

// PMK and LMK keys
static const char *PMK_KEY_STR = PMK_KEY;
static const char *LMK_KEY_STR = LMK_KEY;

// This needs to be global.
// https://rntlab.com/question/espnow-peer-interface-is-invalid/
esp_now_peer_info_t masterInfo;


// callback function executed when data is received
void OnRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    memcpy(&packet, incomingData, sizeof(packet));
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("Temperature: ");
    Serial.println(packet.temperature);
    Serial.print("Humidity: ");
    Serial.println(packet.humidity);
    Serial.print("Pressure: ");
    Serial.println(packet.pressure);

}

void initESPNow() {
    Serial.println("Initializing ESP Now");
    WiFi.mode(WIFI_STA);
    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("There was an error initializing ESP-NOW");
        return;
    }

    // Setting the PMK key
    esp_now_set_pmk((uint8_t *) PMK_KEY_STR);

    // Register the sender
    memcpy(masterInfo.peer_addr, masterAddress, 6);
    masterInfo.channel = 0;
    // Setting the sender device LMK key
    for (uint8_t i = 0; i < 16; i++) {
        masterInfo.lmk[i] = LMK_KEY_STR[i];
    }
    masterInfo.encrypt = true;

    // Add sender
    if (esp_now_add_peer(&masterInfo) != ESP_OK) {
        Serial.println("There was an error registering the sender");
        return;
    }

    esp_now_register_recv_cb(OnRecv);

    Serial.println("Successfully initialized ESP Now");
}

void deinitESPNow() {
    Serial.println("Deinitializing ESP Now");
    esp_now_unregister_recv_cb();
    esp_now_del_peer(masterAddress);
    esp_now_deinit();
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    Serial.println("Successfully deinitialized ESP Now");
}

float getOutsideTemp() {
    return packet.temperature;
}

float getOutsidePress() {
    return packet.pressure;
}

float getOutsideHumid() {
    return packet.humidity;
}