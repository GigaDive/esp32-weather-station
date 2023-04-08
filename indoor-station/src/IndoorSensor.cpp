#include <EEPROM.h>
#include "IndoorSensor.h"
#include "bsec.h"

/* Configure the BSEC library with information about the sensor
    18v/33v = Voltage at Vdd. 1.8V or 3.3V
    3s/300s = BSEC operating mode, BSEC_SAMPLE_RATE_LP or BSEC_SAMPLE_RATE_ULP
    4d/28d = Operating age of the sensor in days
    generic_18v_3s_4d
    generic_18v_3s_28d
    generic_18v_300s_4d
    generic_18v_300s_28d
    generic_33v_3s_4d
    generic_33v_3s_28d
    generic_33v_300s_4d
    generic_33v_300s_28d
*/

const uint8_t bsec_config_iaq[] = {
#include "config/generic_33v_3s_28d/bsec_iaq.txt"
};

#define STATE_SAVE_PERIOD UINT32_C(360 * 60 * 1000) // 360 minutes - 4 times a day

// Helper functions declarations
void checkIaqSensorStatus(void);
void errLeds(void);
void loadState(void);
void updateState(void);

// Create an object of the class Bsec
Bsec iaqSensor;
uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
uint16_t stateUpdateCounter = 0;

String output;

// Entry point for the example
void initIndoorSensor(void)
{
  EEPROM.begin(BSEC_MAX_STATE_BLOB_SIZE + 1); // 1st address for the length
  Wire.begin();

  pinMode(LED_BUILTIN, OUTPUT);
  iaqSensor.begin(BME68X_I2C_ADDR_LOW, Wire);
  output = "\nBSEC library version " + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
  Serial.println(output);
  checkIaqSensorStatus();

  iaqSensor.setConfig(bsec_config_iaq);
  checkIaqSensorStatus();

  loadState();

  bsec_virtual_sensor_t sensorList[10] = {
      BSEC_OUTPUT_RAW_TEMPERATURE,
      BSEC_OUTPUT_RAW_PRESSURE,
      BSEC_OUTPUT_RAW_HUMIDITY,
      BSEC_OUTPUT_RAW_GAS,
      BSEC_OUTPUT_IAQ,
      BSEC_OUTPUT_STATIC_IAQ,
      BSEC_OUTPUT_CO2_EQUIVALENT,
      BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
      BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
      BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
  };

  iaqSensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
  checkIaqSensorStatus();

  // Print the header
  output = "Timestamp [ms], raw temperature [°C], temperature [°C], pressure [hPa], raw relative humidity [%], relative humidity [%], gas [Ohm], IAQ, IAQ accuracy, Static IAQ, CO2 equivalent, breath VOC equivalent";
  Serial.println(output);
}

// Function that is looped forever
void loopIndoorSensor(void)
{
  unsigned long time_trigger = millis();
  if (iaqSensor.run())
  { // If new data is available
    output = String(time_trigger);
    output += ", raw temperature [°C]: " + String(iaqSensor.rawTemperature);
    output += ", temperature [°C]: " + String(iaqSensor.temperature);
    output += ", pressure [hPa]: " + String(iaqSensor.pressure);
    output += ", raw relative humidity [%]: " + String(iaqSensor.rawHumidity);
    output += ", relative humidity [%]: " + String(iaqSensor.humidity);
    output += ", gas [Ohm]: " + String(iaqSensor.gasResistance);
    output += ", IAQ: " + String(iaqSensor.iaq);
    output += ", IAQ accuracy: " + String(iaqSensor.iaqAccuracy);
    output += ", Static IAQ: " + String(iaqSensor.staticIaq);
    output += ", CO2 equivalent: " + String(iaqSensor.co2Equivalent);
    output += ", breath VOC equivalent: " + String(iaqSensor.breathVocEquivalent);
    Serial.println(output);
  }
  else
  {
    checkIaqSensorStatus();
  }
}

// Helper function definitions
void checkIaqSensorStatus(void)
{
  if (iaqSensor.bsecStatus != BSEC_OK)
  {
    if (iaqSensor.bsecStatus < BSEC_OK)
    {
      output = "BSEC error code : " + String(iaqSensor.bsecStatus);
      Serial.println(output);
      for (;;)
        errLeds(); // Halt in case of failure
    }
    else
    {
      output = "BSEC warning code : " + String(iaqSensor.bsecStatus);
      Serial.println(output);
    }
  }

  if (iaqSensor.bme68xStatus != BME68X_OK)
  {
    if (iaqSensor.bme68xStatus < BME68X_OK)
    {
      output = "BME680 error code : " + String(iaqSensor.bme68xStatus);
      Serial.println(output);
      for (;;)
        errLeds(); // Halt in case of failure
    }
    else
    {
      output = "BME680 warning code : " + String(iaqSensor.bme68xStatus);
      Serial.println(output);
    }
  }
  iaqSensor.bsecStatus = BSEC_OK;
}

void errLeds(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}

void loadState(void)
{
  if (EEPROM.read(0) == BSEC_MAX_STATE_BLOB_SIZE)
  {
    // Existing state in EEPROM
    Serial.println("Reading state from EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++)
    {
      bsecState[i] = EEPROM.read(i + 1);
      Serial.println(bsecState[i], HEX);
    }

    iaqSensor.setState(bsecState);
    checkIaqSensorStatus();
  }
  else
  {
    // Erase the EEPROM with zeroes
    Serial.println("Erasing EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE + 1; i++)
      EEPROM.write(i, 0);

    EEPROM.commit();
  }
}

void updateState(void)
{
  bool update = false;
  if (stateUpdateCounter == 0) {
    /* First state update when IAQ accuracy is >= 3 */
    if (iaqSensor.iaqAccuracy >= 3) {
      update = true;
      stateUpdateCounter++;
    }
  } else {
    /* Update every STATE_SAVE_PERIOD minutes */
    if ((stateUpdateCounter * STATE_SAVE_PERIOD) < millis()) {
      update = true;
      stateUpdateCounter++;
    }
  }

  if (update)
  {
    iaqSensor.getState(bsecState);
    checkIaqSensorStatus();

    Serial.println("Writing state to EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++)
    {
      EEPROM.write(i + 1, bsecState[i]);
      Serial.println(bsecState[i], HEX);
    }

    EEPROM.write(0, BSEC_MAX_STATE_BLOB_SIZE);
    EEPROM.commit();
  }
}

float getIndoorTemp() { return iaqSensor.temperature; }
float getIndoorPress() { return iaqSensor.pressure; }
float getIndoorHumid() { return iaqSensor.humidity; }
int getIndoorAQI() { return iaqSensor.iaq; }
int getIndoorAQIAccuracy() { return iaqSensor.iaqAccuracy; }

