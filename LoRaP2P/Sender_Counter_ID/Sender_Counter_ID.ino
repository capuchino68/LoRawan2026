#include <SPI.h>
#include <LoRa.h>

#define NETWORK_ID 0xA5//ID RED de nodos
#define SRC_ID     1//ID remitente
#define DST_ID     6//ID destinatario

#define LORA_FREQ 915E6

uint16_t counter = 0;

void setup() {
  Serial.begin(9600);

  pinMode(RFM_TCX_ON, OUTPUT);
  pinMode(RFM_SWITCH, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(RFM_TCX_ON, HIGH);
  digitalWrite(RFM_SWITCH, HIGH);

  LoRa.setPins(SS, RFM_RST, RFM_DIO0);

  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("LoRa failed");
    while (1);
  }

  LoRa.setSyncWord(0x12);
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setTxPower(17);

  Serial.println("Emisor listo");
}

void loop() {
  uint8_t payload[5];

  payload[0] = NETWORK_ID;
  payload[1] = SRC_ID;
  payload[2] = DST_ID;
  payload[3] = highByte(counter);
  payload[4] = lowByte(counter);

  Serial.print("Enviando counter: ");
  Serial.println(counter);

  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(RFM_SWITCH, HIGH);

  LoRa.beginPacket();
  LoRa.write(payload, sizeof(payload));
  LoRa.endPacket();

  digitalWrite(LED_BUILTIN, LOW);

  counter++;

  delay(5000);
}
