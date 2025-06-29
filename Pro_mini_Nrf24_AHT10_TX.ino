/*
  Wireless Sensor Transmitter – Temp, Humidity, Battery Voltage
  Based on Arduino Pro Mini 3.3V 8MHz + NRF24L01

  ──────────────────────────────────────────────
  📦 Components:
  - Arduino Pro Mini 3.3V 8MHz
  - AHT10 Temperature & Humidity Sensor (I2C)
  - NRF24L01 Wireless Transceiver
  - Voltage Divider: 320Ω (top) + 100Ω (bottom) for battery voltage reading
  - HT7333 LDO Regulator for 3.3V from 3.7V Li-ion battery
  - Capacitors:
      - 1µF on HT7333 IN and OUT
      - 10–100µF near NRF24L01 for stability

  🔌 Wiring:
  - AHT10:
      SDA → A4
      SCL → A5
      VCC → 3.3V
      GND → GND

  - NRF24L01:
      CE   → D9  
      CSN  → D10  
      MOSI → D11  
      MISO → D12  
      SCK  → D13  
      VCC  → 3.3V  
      GND  → GND  

  - Voltage Divider:
      Battery + → 320KΩ → A0 → 100KΩ → GND

  - Power:
      Li-ion 3.7V battery → HT7333 IN
      HT7333 OUT → 3.3V to all components
      GND shared between all modules

  🔋 Function:
  - Reads temperature, humidity, and battery voltage
  - Sends data via NRF24L01
  - Sleeps ~60 seconds between transmissions using LowPower library

 * Written by Amir — April 2025. Enjoy!
 ******************************************************************************/



#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <SPI.h>
#include <RF24.h>
#include <LowPower.h>

Adafruit_AHTX0 aht;
RF24 radio(9, 10); // CE, CSN
const byte address[6] = "00001";

// פרמטר כיול למתח
const float voltageCalibrationFactor = 1.004; // שנה כאן לפי הצורך

struct SensorData {
  float temperature;
  float humidity;
  float batteryVoltage;
};

float readBatteryVoltage() {
  analogReference(INTERNAL);
  delay(5);

  int raw = analogRead(A0);
  float vRef = 1.1;

  float measuredVoltage = (raw / 1023.0) * vRef;
  float actualVoltage = measuredVoltage * (320.0 + 100.0) / 100.0;

  return actualVoltage * voltageCalibrationFactor;
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!radio.begin()) {
    Serial.println("NRF24L01 not detected");
    while (1);
  }

  if (!aht.begin()) {
    Serial.println("AHT10 not found");
    while (1);
  }

  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();

  Serial.println("Transmitter ready");
}

void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  float batteryVoltage = readBatteryVoltage();

  SensorData dataToSend;
  dataToSend.temperature = temp.temperature;
  dataToSend.humidity = humidity.relative_humidity;
  dataToSend.batteryVoltage = batteryVoltage;

  bool success = radio.write(&dataToSend, sizeof(dataToSend));

  if (success) {
    Serial.print("Sent | Temp: ");
    Serial.print(dataToSend.temperature);
    Serial.print(" °C | Hum: ");
    Serial.print(dataToSend.humidity);
    Serial.print(" % | Bat: ");
    Serial.print(dataToSend.batteryVoltage);
    Serial.println(" V");
  } else {
    Serial.println("Failed to send");
  }

  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
}
