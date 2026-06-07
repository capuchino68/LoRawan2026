#include <SPI.h>
#include <LoRa.h>

void setup() {
  Serial.begin(9600);

  pinMode(RFM_TCX_ON, OUTPUT);
  pinMode(RFM_SWITCH, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(RFM_TCX_ON, HIGH);
  digitalWrite(RFM_SWITCH, HIGH);

  LoRa.setPins(SS, RFM_RST, RFM_DIO0);

  if (!LoRa.begin(915E6)) {
    Serial.println("LoRa failed");
    while (1);
  }

  Serial.println("LoRa Receiver OK");
}

void loop() {
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    String received = "";

    while (LoRa.available()) {
      received += (char)LoRa.read();
    }

    int rssi = LoRa.packetRssi();
    float snr = LoRa.packetSnr();

    Serial.print("Temp recibida: ");
    Serial.print(received);
    Serial.print(" C | RSSI: ");
    Serial.print(rssi);
    Serial.print(" dBm | SNR: ");
    Serial.print(snr);
    Serial.println(" dB");

    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
  }
}
