# ESP32 Weather-Station
An implementation for a weather station on Espressif's ESP32 and ESP32-S2, as well as a Waveshare E-Ink Display.  
The project concerns two parts. A battery operated outside unit, where low power-consumption is the main focus and an inside unit which has the E-Ink display attatched.
Environmental measurements are done via the Bosch BME 280 and BME 680 respectively.

## Outdoor Sensor
The outdoor sensor is a ESP32-S2, takes its measurements via the BME280 sensor and transmits them to an MQTT-Server via WiFi.  
The current sequence goes as follows:  
1. Wake up from Hibernation, turn off the WiFi-module and lower the CPU frequency to conserve power
2. Take all the necessary measurements with the BME280.
3. Reenable WiFi again and connect to an access-point
4. Upload all the data to their relevant MQTT-channels
5. Enter Hibernation again for a set amount of time

For more information on hibernation, see [this blog post] (https://www.mischianti.org/2021/03/15/esp32-practical-power-saving-deep-sleep-and-hibernation-3/#Hibernation)
In the future, we might switch to ESP-Now for the transmission, but we need some conclusive power consumption measurements first.

## Inside Station
TODO
