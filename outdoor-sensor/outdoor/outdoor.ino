#include <WiFi.h>
#include <esp_now.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme; // Using the I2C-protocol, no further setup necessary

#define uS_TO_S_FACTOR 1000000 // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 600       // Time ESP32 will go to sleep (in seconds)

// Set the SLAVE MAC Address (Receiver)
uint8_t slaveAddress[] = {0x34, 0xAB, 0x95, 0x48, 0x92, 0x94};

// PMK and LMK keys
static const char *PMK_KEY_STR = "PLEASE_CHANGE_ME";
static const char *LMK_KEY_STR = "DONT_BE_LAZY_OK?";

// Structure to keep the temperature, humidity and pressure data from the sensor
struct temp_humidity_pressure
{
  float temperature;
  float humidity;
  float pressure;
};

temp_humidity_pressure packet;

/* Error-Codes:
  1: DEVICE_ID not set
  30: BME280 sensor not properly connected; Check wiring
  10: There was an error initializing ESP-NOW
  11: There was an error registering the slave/receiver
  20: Too many retries while trying to send data-packet via ESPNow
*/

void setModemSleep()
{
  disableWiFi();
  // setCpuFrequencyMhz(40);
  // enabling this scrambles the debug console, use later in production
}

int wakeModemSleep()
{
  // setCpuFrequencyMhz(80);
  return enableWiFi();
}

void disableWiFi()
{
  WiFi.disconnect(true); // Disconnect from the network
  WiFi.mode(WIFI_OFF);   // Switch Wi-Fi off
  Serial.println("");
  Serial.println("WiFi-Module disabled");
}

int enableWiFi()
{
  WiFi.disconnect(false); // Reconnect the network
  WiFi.persistent(false); // Block permanent writing to flash storage. See http://www.forum-raspberrypi.de/Thread-esp8266-achtung-flash-speicher-schreibzugriff-bei-jedem-aufruf-von-u-a-wifi-begin
  WiFi.mode(WIFI_STA);    // Switch WiFi-AP off

  Serial.println("WiFi-Module enabled");
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("There was an error initializing ESP-NOW");
    return 10;
  }
  // Setting the PMK key
  esp_now_set_pmk((uint8_t *)PMK_KEY_STR);

  // Register the slave
  esp_now_peer_info_t slaveInfo;
  memcpy(slaveInfo.peer_addr, slaveAddress, 6);
  slaveInfo.channel = 0;

  // Setting the master device LMK key
  for (uint8_t i = 0; i < 16; i++)
  {
    slaveInfo.lmk[i] = LMK_KEY_STR[i];
  }
  slaveInfo.encrypt = true;

  // Add slave
  if (esp_now_add_peer(&slaveInfo) != ESP_OK)
  {
    Serial.println("There was an error registering the slave");
    return 11;
  }

  return 0;
}

int readBMEValues()
{
  bool status;

  status = bme.begin(0x76);
  if (!status)
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    return 30;
  }

  // weather monitoring
  Serial.println("-- Weather Station Scenario --");
  Serial.println("forced mode, 2x temperature / 2x humidity / 2x pressure oversampling,");
  Serial.println("filter off");
  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X2, // temperature
                  Adafruit_BME280::SAMPLING_X2, // pressure
                  Adafruit_BME280::SAMPLING_X2, // humidity
                  Adafruit_BME280::FILTER_OFF);

  bme.takeForcedMeasurement();

  packet.temperature = bme.readTemperature();
  packet.humidity = bme.readHumidity();
  packet.pressure = bme.readPressure();

  Serial.printf("Temperature = %2.1lf °C \n", packet.temperature);
  Serial.printf("Pressure = %6.2lf hPa \n", packet.pressure);
  Serial.printf("Humidity = %3.2lf % \n", packet.humidity);
  Serial.println();

  Serial.println();
  return 0;
}

int sendValuesViaESPNow()
{
  // It's time to send the message via ESP-NOW ...now
  esp_err_t result = esp_now_send(slaveAddress, (uint8_t *)&packet, sizeof(packet));

  // delay(100);

  if (result == ESP_OK)
  {
    Serial.println("The message was sent successfully.");
    return 0;
  }
  else
  {
    Serial.println("There was an error sending the message.");
    return 20;
  }
}

void hibernate()
{
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

// Arduino Setup function. Called everytime after an esp_deep_sleep
void setup()
{
  Serial.begin(115200); // Defining the channel on which logging is done
  int error;            // All relevant functions can return an error-code which is saved here

  Serial.println("Hello! I'm awake again (つ ◕_◕ )つ");

  // 1. Turn off wireless communication
  setModemSleep();

  // 2. Read out all sensor values from the BME280
  error = readBMEValues();

  // 3. Enable wireless communication again
  if (!error)
  {
    error = wakeModemSleep();
  }

  // 4. Send the sensor values to the indoor station via ESP-NOW
  if (!error)
  {
    error = sendValuesViaESPNow();
  }

  // 5. Go to sleep again
  setModemSleep();
  Serial.println((error) ? "There were errors during this run. EC: " + String(error) : "No errors, oh yeah! (⌐■_■)");
  Serial.println("Now hibernating for " + String(TIME_TO_SLEEP) + " seconds. (∪.∪ )...zzz.");
  hibernate();
}

void loop()
{
}
