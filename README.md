# Nano_Nrf24_AHT10_TX-RX-Mqtt

### Wireless Environmental Monitoring System with nRF24L01+

This project is a lightweight, battery-friendly wireless monitoring system using **Arduino**, **AHT10**, and **nRF24L01+** modules. It includes a single transmitter and two types of receivers:

![image alt](https://github.com/amir684/Nrf24_AHT10_TX-RX/blob/main/images/20250629_215059.jpg)

![image alt](https://github.com/amir684/Nrf24_AHT10_TX-RX/blob/main/images/20250629_214156.jpg)

---

## 🛰️ Components

### 1. **Transmitter**  
- **Hardware**: Arduino Pro Mini 3.3V 8MHz + AHT10 + nRF24L01+  
- **Function**: Reads temperature, humidity, and battery voltage. Sends packets over nRF24.  
- **Power Efficient**: Designed for long-term battery operation.

![image alt](https://github.com/amir684/Nrf24_AHT10_TX-RX/blob/main/images/20250629_215127.jpg)


### 2. **Receiver with OLED**  
- **Hardware**: Arduino Nano + nRF24L01+ + OLED 128x32  
- **Function**: Receives and displays data in real-time:  
  • Temperature (°C)  
  • Humidity (%)  
  • Battery Voltage (V)  
  • Message Count & Timeout Since Last Message

![image alt](https://github.com/amir684/Nrf24_AHT10_TX-RX/blob/main/images/Screenshot%202025-06-29%20215225.png)

### 3. **Receiver with MQTT**  
- **Hardware**: ESP8266 (e.g. Wemos D1 Mini) + nRF24L01+  
- **Function**: Receives packets and publishes them to an MQTT broker in JSON format for Home Assistant or similar platforms.

---

## 📡 Communication
All nodes use **nRF24L01+** and a shared address (pipe 0) for data transmission.

---

## 🏠 Use Case Examples
- Remote temperature & humidity sensing
- Smart home monitoring with MQTT
- Data logging for environmental analysis

---

## 📁 Code Files
- `TX_ProMini_AHT10.ino`: Battery-powered transmitter
- `RX_Nano_OLED.ino`: Arduino Nano receiver with OLED
- `RX_ESP8266_MQTT.ino`: ESP8266 receiver publishing to MQTT

---

## 🛠️ Dependencies
- RF24 by TMRh20
- Adafruit AHT10 / Wire
- Adafruit SSD1306 / Adafruit GFX
- PubSubClient (for ESP8266 MQTT)

---

## 🌍 License
Public Domain / CC0. Use freely for any purpose.

---

Created by **AMIR** — June 2025
