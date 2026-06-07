#include <SPI.h>
#include <LoRa.h>

#define NETWORK_ID 0xA5
#define MY_ID      6
#define PEER_ID    1

#define LORA_FREQ 915E6

#define LED_VERDE 6
#define LED_ROJO  7

void paqueteValido() {
  digitalWrite(LED_ROJO, LOW);

  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_VERDE, HIGH);
    delay(100);
    digitalWrite(LED_VERDE, LOW);
    delay(100);
  }

  digitalWrite(LED_VERDE, HIGH);
}

void paqueteIgnorado() {
  digitalWrite(LED_VERDE, LOW);

  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_ROJO, HIGH);
    delay(100);
    digitalWrite(LED_ROJO, LOW);
    delay(100);
  }

  digitalWrite(LED_ROJO, HIGH);
}

void setup() {
  Serial.begin(9600);

  pinMode(RFM_TCX_ON, OUTPUT);
  pinMode(RFM_SWITCH, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);

  digitalWrite(RFM_TCX_ON, HIGH);
  digitalWrite(RFM_SWITCH, LOW);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_ROJO, LOW);

  LoRa.setPins(SS, RFM_RST, RFM_DIO0);

  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("LoRa failed");
    while (1);
  }

  LoRa.setSyncWord(0x12);
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);

  Serial.println("Receptor listo");
}

void loop() {
  digitalWrite(RFM_SWITCH, LOW);

  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    if (packetSize < 5) {
      Serial.println("Paquete corto ignorado");
      while (LoRa.available()) LoRa.read();
      paqueteIgnorado();
      return;
    }

    uint8_t buffer[5];

    for (int i = 0; i < 5; i++) {
      buffer[i] = LoRa.read();
    }

    uint8_t netId = buffer[0];
    uint8_t srcId = buffer[1];
    uint8_t dstId = buffer[2];

    if (netId != NETWORK_ID) {
      Serial.println("Red incorrecta");
      paqueteIgnorado();
      return;
    }

    if (dstId != MY_ID) {
      Serial.println("No es para este nodo");
      paqueteIgnorado();
      return;
    }

    if (srcId != PEER_ID) {
      Serial.println("Nodo no autorizado");
      paqueteIgnorado();
      return;
    }

    uint16_t counter = ((uint16_t)buffer[3] << 8) | buffer[4];

    paqueteValido();

    Serial.println("Paquete valido recibido");
    Serial.print("De nodo: ");
    Serial.println(srcId);
    Serial.print("Counter: ");
    Serial.println(counter);
    Serial.print("RSSI: ");
    Serial.println(LoRa.packetRssi());
    Serial.print("SNR: ");
    Serial.println(LoRa.packetSnr());
  }
}
