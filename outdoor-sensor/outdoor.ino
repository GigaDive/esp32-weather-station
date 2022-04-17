#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);
Adafruit_BME280 bme; // Using the I2C-protocol, no further setup neccessary

#define uS_TO_S_FACTOR 1000000 //Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 60       //Time ESP32 will go to sleep (in seconds)
int max_connect_retries = 3;

String MQTT_CHANNEL_OUTDOOR_TEMPERATURE;
String MQTT_CHANNEL_OUTDOOR_HUMIDITY;
String MQTT_CHANNEL_OUTDOOR_AIR_PRESSURE;

char BME_TEMPERATURE[5];
char BME_HUMIDITY[5];
char BME_AIR_PRESSURE[7];

const String DEVICE_ID = "SuperSensor";
const int MQTT_PORT = 8883;
const String MQTT_SERVER = "fynf.dev";
const String MQTT_USER = "";
const String MQTT_PASSWORD = "";

const String ssid = "";                      // WiFi name
const String password = ""; // WiFi password
const static char *isrg_root_ca PROGMEM =
    "-----BEGIN CERTIFICATE-----\n"
    "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\n"
    "-----END CERTIFICATE-----"; // ISRG Root X1 - Self signed, valid until Jun. 4 11:04:38 2035 GMT

/* Error-Codes: 
    1: DEVICE_ID not set
    10: Too many retries while connecting to the MQTT-server
    11: Too many retries while uploading MQTT-data
    20: WiFi disconnect before or during MQTT-data-upload
    21: Unable to connect to WiFi-AP 
    30: BME280 sensor not properly connected; Check wiring
*/

int connectMQTT()
{
    mqttClient.setServer(MQTT_SERVER.c_str(), MQTT_PORT);
    int retries = 0;

    while (!mqttClient.connected() && retries < max_connect_retries)
    {
        if (DEVICE_ID.length() <= 0)
        {
            Serial.print("Device ID was not set. Not attempting MQTT-connection.");
            return 1;
        }

        Serial.print("Now connecting to MQTT-Server at: ");
        Serial.println(MQTT_SERVER);
        if (mqttClient.connect(DEVICE_ID.c_str(), MQTT_USER.c_str(), MQTT_PASSWORD.c_str()))
        {
            Serial.println("Connected with device-id: " + String(DEVICE_ID));
            return 0;
        }
        else
        {
            Serial.println("Connection failed: " + String(mqttClient.state()));
            retries++;
            delay((1024 + random(0, 50)) * retries);
        }
    }
    return 10; // Couldn't conect to MQTT-server
}

int uploadMQTT()
{
    MQTT_CHANNEL_OUTDOOR_TEMPERATURE = String(String("/sensor/weather/") + String(DEVICE_ID) + String("/TEMPERATURE")).c_str();
    MQTT_CHANNEL_OUTDOOR_HUMIDITY = String(String("/sensor/weather/") + String(DEVICE_ID) + String("/MOISTURE")).c_str();
    MQTT_CHANNEL_OUTDOOR_AIR_PRESSURE = String(String("/sensor/weather/") + String(DEVICE_ID) + String("/AIR_PRESSURE")).c_str();

    // WiFi needs to be connected, else we won't attempt anything
    if (WiFi.status() == WL_CONNECTED)
    {
        int retries = 0;
        while (retries < max_connect_retries)
        {
            // In case we disconnected from the server, try to connect again last minute
            if (!mqttClient.connected())
            {
                connectMQTT();
                retries++;
                Serial.print("Retrying MQTT connection. Retires left: ");
                Serial.println(max_connect_retries - retries);
            }
            else
            {
                Serial.println("Now sending MQTT-messages to the server...");
                //mqttClient.publish(channel(String), value(String), retained(bool))
                mqttClient.publish(MQTT_CHANNEL_OUTDOOR_TEMPERATURE.c_str(), BME_TEMPERATURE, true);
                mqttClient.publish(MQTT_CHANNEL_OUTDOOR_HUMIDITY.c_str(), BME_HUMIDITY, true);
                mqttClient.publish(MQTT_CHANNEL_OUTDOOR_AIR_PRESSURE.c_str(), BME_AIR_PRESSURE, true);
                mqttClient.disconnect();
                int retries = 0;
                while (mqttClient.connected() && retries < max_connect_retries)
                {
                    retries++;
                    delay((256 + random(0, 50)) * retries);
                    Serial.print(".");
                }
                Serial.println("Done.");
                return 0;
            }
        }
        return 11; // Couldn't reconect to MQTT-server
    }
    else
    {
        return 20; // WiFi disconnected
    }
}

void disableWiFi()
{
    WiFi.disconnect(true); // Disconnect from the network
    WiFi.mode(WIFI_OFF);   // Switch WiFi off
    Serial.println("");
    Serial.println("WiFi disconnected");
}

void setModemSleep()
{
    disableWiFi();
    //setCpuFrequencyMhz(40);
    // enabling this scrambles the debug console, use later in production
}

int enableWiFi()
{
    WiFi.disconnect(false); // Reconnect the network
    WiFi.persistent(false); // Block permanent writing to flash storage. See http://www.forum-raspberrypi.de/Thread-esp8266-achtung-flash-speicher-schreibzugriff-bei-jedem-aufruf-von-u-a-wifi-begin
    WiFi.mode(WIFI_STA);    // Switch WiFi-AP off
    wifiClient.setCACert(isrg_root_ca);

    delay(200);

    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid.c_str(), password.c_str());

    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < max_connect_retries)
    {
        retries++;
        delay((1024 + random(0, 50)) * retries);
        Serial.print(".");
    }

    // If WiFi's still not connected after a few retries, we abort
    if (WiFi.status() != WL_CONNECTED)
    {
        return 21;
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.print(WiFi.localIP());
    Serial.print("RRSI: ");
    Serial.println(WiFi.RSSI());
    Serial.println("");
    return 0;
}

int wakeModemSleep()
{
    //setCpuFrequencyMhz(240);
    return enableWiFi();
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

    //dtostrf(value(float), mininum width, precision, target
    dtostrf(bme.readTemperature(), 4, 2, BME_TEMPERATURE);
    Serial.print("Temperature = ");
    Serial.print(BME_TEMPERATURE);
    Serial.println(" *C");

    dtostrf(bme.readPressure(), 6, 2, BME_AIR_PRESSURE);
    Serial.print("Pressure = ");
    Serial.print(BME_AIR_PRESSURE);
    Serial.println(" hPa");

    dtostrf(bme.readHumidity(), 4, 2, BME_HUMIDITY);
    Serial.print("Humidity = ");
    Serial.print(BME_HUMIDITY);
    Serial.println(" %");

    Serial.println();
    return 0;
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
    int error;            // All relevant functions may return an error-code which is saved here

    // Say hello
    Serial.println("Hello! I'm awake again (つ ◕_◕ )つ");
    delay(100);

    // 1. Turn off wireless communication
    setModemSleep();

    // 2. Read out all sensor values from the BME280
    error = readBMEValues();

    // 3. Enable wireless comunication again
    if (!error)
    {
        error = wakeModemSleep();
    }

    // 4. Send the sensor values to the MQTT service
    if (!error)
    {
        error = connectMQTT();
    }
    if (!error)
    {
        error = uploadMQTT();
    }
    // 5. Go to sleep again
    setModemSleep();
    Serial.println((error) ? "There were errors during this run. EC: " + String(error) : "No errors, oh yeah! (⌐■_■)");
    Serial.println("Now hibernating for " + String(TIME_TO_SLEEP) + " seconds. (∪.∪ )...zzz.");
    hibernate();
}

void loop()
{
    // Never reached, as we only run all function once before entering hibernation again
}
