/*
  Daniel Carrasco
  This and more tutorials at https://www.electrosoftcloud.com/
*/

#include <esp_now.h>
#include <WiFi.h>

// Set the SLAVE MAC Address
uint8_t masterAddress[] = {0x7C, 0xDF, 0xA1, 0x00, 0xF9, 0xDC};

// PMK and LMK keys
static const char* PMK_KEY_STR = "PLEASE_CHANGE_ME";
static const char* LMK_KEY_STR = "DONT_BE_LAZY_OK?";

// Structure to keep the temperature and humidity data
// Is also required in the client to be able to save the data directly
struct temp_humidity_pressure {
  float temperature;
  float humidity;
  float pressure;
};

temp_humidity_pressure packet;

// this needs to be global. Thank you, Steve and Sara
// https://rntlab.com/question/espnow-peer-interface-is-invalid/
esp_now_peer_info_t masterInfo;

// callback function executed when data is received
void OnRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&packet, incomingData, sizeof(packet));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Temperature: ");
  Serial.println(packet.temperature);
  Serial.print("Humidity: ");
  Serial.println(packet.humidity);
  Serial.print("Pessure: ");
  Serial.println(packet.pressure);
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("There was an error initializing ESP-NOW");
    return;
  }

  // Setting the PMK key
  esp_now_set_pmk((uint8_t *)PMK_KEY_STR);

  // Register the master
  memcpy(masterInfo.peer_addr, masterAddress, 6);
  masterInfo.channel = 0;
  // Setting the master device LMK key
  for (uint8_t i = 0; i < 16; i++) {
    masterInfo.lmk[i] = LMK_KEY_STR[i];
  }
  masterInfo.encrypt = true;

  // Add master
  if (esp_now_add_peer(&masterInfo) != ESP_OK) {
    Serial.println("There was an error registering the master");
    return;
  }

  // Once the ESP-Now protocol is initialized, we will register the callback function
  // to be able to react when a package arrives in near to real time without pooling every loop.
  esp_now_register_recv_cb(OnRecv);
}

void loop() {
}
