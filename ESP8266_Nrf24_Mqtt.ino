/******************************************************************************
 * Nano_Nrf24_AHT10_RX_ESP8266 — RF-to-MQTT Gateway
 *
 * Overview
 * --------
 * This sketch converts an **ESP8266** (NodeMCU / Wemos D1 mini) into a bridge
 * that:
 *   1. Listens for wireless packets from a Nano-based sensor node that sends
 *      temperature, humidity, and battery voltage over **nRF24L01+**.
 *   2. Publishes the received values as a single JSON message to an MQTT broker
 *      on your local network, so Home Assistant (or any MQTT dashboard) can
 *      consume them in real time.
 *
 * Data Flow
 * ---------
 *  nRF24L01+  →  ESP8266  →  Wi-Fi  →  MQTT topic **sensors/room1**
 *  (Temp, Hum, Vbat)        JSON payload: {"temperature":..,"humidity":..,"battery":..}
 *
 * Hardware Connections
 * --------------------
 *   nRF24L01+   →  ESP8266 (Wemos D1 mini pinout shown)
 *     CE  ——  D2   (GPIO4)
 *     CSN ——  D1   (GPIO5)
 *     MOSI——  D7   (GPIO13)
 *     MISO——  D6   (GPIO12)
 *     SCK ——  D5   (GPIO14)
 *     VCC ——  3V3  (add 10 µF capacitor close to the module)
 *     GND ——  GND
 *
 * Key Features
 * ------------
 * • **Automatic Wi-Fi & MQTT reconnect** — keeps the link alive even after
 *   outages.  
 * • **Compact JSON payload** — easy to parse in Home Assistant with one sensor
 *   per key.  
 * • **Adjustable RF power** — set to `RF24_PA_MAX` here for range; lower it if
 *   you need to save current or reduce interference.  
 *
 * Security Note
 * -------------
 * Wi-Fi SSID, Wi-Fi password, and MQTT credentials are hard-coded for clarity.
 * Replace them with your own **secure secrets** or move them to a separate
 * header file (`secrets.h`) before committing to a public repo.
 *
 * Dependencies
 * ------------
 *   • RF24 by TMRh20  
 *   • ESP8266WiFi (built-in with ESP8266 core)  
 *   • PubSubClient by Nick O’Leary  
 *
 * Written by Amir — April 2025. Enjoy!
 ******************************************************************************/



#include <SPI.h>
#include <RF24.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// הגדרות WiFi
const char* ssid = "amir&liel";
const char* password = "amir3080";

// הגדרות MQTT
const char* mqtt_server = "192.168.1.175";
const char* mqtt_user = "mqtt_user";       // שם משתמש MQTT
const char* mqtt_password = "amir3080";   // סיסמת MQTT

WiFiClient espClient;
PubSubClient client(espClient);

// הגדרות NRF24
RF24 radio(D2, D1); // CE, CSN

const byte address[6] = "00001";

// מבנה הנתונים שמתקבל
struct SensorData {
  float temperature;
  float humidity;
  float batteryVoltage;
};

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP8266Receiver", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  delay(1000);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");

  client.setServer(mqtt_server, 1883);

  if (!radio.begin()) {
    Serial.println("NRF24L01 not detected");
    while (1);
  }

  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();

  Serial.println("Receiver ready");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (radio.available()) {
    SensorData receivedData;
    radio.read(&receivedData, sizeof(receivedData));

    Serial.print("Received | Temp: ");
    Serial.print(receivedData.temperature);
    Serial.print(" °C | Hum: ");
    Serial.print(receivedData.humidity);
    Serial.print(" % | Bat: ");
    Serial.print(receivedData.batteryVoltage);
    Serial.println(" V");

    // יצירת JSON ושליחה ל-MQTT
    String payload = "{";
    payload += "\"temperature\":" + String(receivedData.temperature, 1) + ",";
    payload += "\"humidity\":" + String(receivedData.humidity, 1) + ",";
    payload += "\"battery\":" + String(receivedData.batteryVoltage, 2);
    payload += "}";

    client.publish("sensors/room1", payload.c_str());
  }
}
