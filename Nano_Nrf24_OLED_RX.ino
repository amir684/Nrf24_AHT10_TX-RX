/******************************************************************************
 * Nano_Nrf24_AHT10_RX — Wireless Environmental Monitor (Receiver)
 *
 * Overview
 * --------
 * This sketch turns an **Arduino Nano** into a low-power receiver node that
 * listens for packets sent over **nRF24L01+** radio from a matching
 * Nano_Nrf24_AHT10_TX transmitter.  
 *
 * Each packet carries:
 *   • Temperature (°C)  
 *   • Humidity (%)  
 *   • Battery voltage (V)  
 *
 * On every packet the data are:
 *   1. Logged to the Serial Monitor (9600 baud).  
 *   2. Shown on a 128 × 32 I²C **SSD1306 OLED** in three lines  
 *      – temperature & humidity, battery voltage, message counter & time-since-last.  
 *
 * Hardware Connections
 * --------------------
 *   nRF24L01+   →  Nano  
 *     CE  ——  9  
 *     CSN —— 10  
 *     MOSI—— 11 (HW-SPI)  
 *     MISO—— 12  
 *     SCK —— 13  
 *     VCC —— 3 V3 (use 10 µF cap nearby)  
 *     GND —— GND  
 *
 *   SSD1306 OLED (I²C, 0x3C) → Nano  
 *     SDA —— A4  
 *     SCL —— A5  
 *     VCC —— 5 V (or 3 V3)  
 *     GND —— GND  
 *
 * Key Features
 * ------------
 * • **Live display refresh** – the screen is updated every loop, so the last
 *   valid reading stays visible even if no new packet arrives.  
 * • **Counter** – increments on every successful reception.  
 * • **Timeout indicator** – shows seconds elapsed since the last packet, useful
 *   for spotting drop-outs or dead transmitters.  
 *
 * Power Tips
 * ----------
 * Running the Nano at 8 MHz/3 V3 and dimming or blanking the OLED between
 * updates can extend battery life in field deployments.
 *
 * Compiler & Libraries
 * --------------------
 *   • Arduino IDE ≥ 1.8 / PlatformIO  
 *   • RF24 by TMRh20  
 *   • Adafruit_GFX + Adafruit_SSD1306  
 *
 * Written by Amir — April 2025. Enjoy!
 ******************************************************************************/


#include <SPI.h>
#include <Wire.h>
#include <RF24.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RF24 radio(9, 10); // CE, CSN

const byte address[6] = "00001";

struct SensorData {
  float temperature;
  float humidity;
  float batteryVoltage;
};

int counter = 0;
unsigned long lastReceiveTime = 0;
SensorData receivedData; // שמירה של ההודעה האחרונה

void setup() {
  Serial.begin(9600);
  while (!Serial);

  delay(1000);

  if (!radio.begin()) {
    Serial.println("NRF24L01 not detected");
    while (1);
  }

  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();

  while (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found, retrying...");
    delay(500);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Waiting for data...");
  display.display();

  Serial.println("Receiver ready");
}

void loop() {
  // אם התקבלה הודעה – קרא ושמור
  if (radio.available()) {
    radio.read(&receivedData, sizeof(receivedData));
    counter++;
    lastReceiveTime = millis();

    Serial.print("Received | Temp: ");
    Serial.print(receivedData.temperature);
    Serial.print(" C | Hum: ");
    Serial.print(receivedData.humidity);
    Serial.print(" % | Vbat: ");
    Serial.print(receivedData.batteryVoltage);
    Serial.println(" V");
  }

  // תצוגה תמידית – גם אם לא התקבלה הודעה חדשה
  display.clearDisplay();
  display.setTextSize(1);

  // שורה 1: טמפ + לחות
  display.setCursor(0, 0);
  display.print("T:");
  display.print(receivedData.temperature, 1);
  display.print("C  H:");
  display.print(receivedData.humidity, 1);
  display.println("%");

  // שורה 2: מתח סוללה
  display.setCursor(0, 10);
  display.print("Bat: ");
  display.print(receivedData.batteryVoltage, 2);
  display.println(" V");

  // שורה 3: מונה
  display.setCursor(0, 20);
  display.print("Count: ");
  display.print(counter);

  // שורה 3 המשך: זמן מאז הודעה אחרונה
  display.setCursor(70, 20);
  unsigned long timeSinceLast = (millis() - lastReceiveTime) / 1000;
  display.print(" T:");
  display.print(timeSinceLast);
  display.print("s");

  display.display();
}
