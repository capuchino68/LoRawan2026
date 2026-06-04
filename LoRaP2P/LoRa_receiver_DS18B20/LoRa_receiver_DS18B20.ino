#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =====================
// SSD1306 OLED 0.96"
// =====================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

void setup() {
  Serial.begin(9600);

  pinMode(RFM_TCX_ON, OUTPUT);
  pinMode(RFM_SWITCH, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(RFM_TCX_ON, HIGH);
  digitalWrite(RFM_SWITCH, HIGH);

  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 failed");
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(10, 20);
  display.println("RX BOOT");
  display.display();

  delay(1000);

  LoRa.setPins(SS, RFM_RST, RFM_DIO0);

  if (!LoRa.begin(915E6)) {
    Serial.println("LoRa failed");

    display.clearDisplay();
    display.setCursor(0, 20);
    display.println("LORA FAIL");
    display.display();

    while (1);
  }

  Serial.println("LoRa Receiver OK");

  display.clearDisplay();
  display.setCursor(15, 20);
  display.println("RX OK");
  display.display();

  delay(1000);
}

void loop() {
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    String received = "";

    while (LoRa.available()) {
      received += (char)LoRa.read();
    }

    int rssi = LoRa.packetRssi();

    Serial.print("Temp recibida: ");
    Serial.print(received);
    Serial.print(" C | RSSI: ");
    Serial.println(rssi);

    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("LoRa RX");

    display.setTextSize(2);
    display.setCursor(0, 18);
    display.print(received);
    display.println(" C");

    display.setTextSize(1);
    display.setCursor(0, 52);
    display.print("RSSI: ");
    display.println(rssi);

    display.display();

    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
  }
}
