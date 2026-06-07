#include <SPI.h>
#include <LoRa.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 5

#define NETWORK_ID 0xA5
#define SRC_ID     1 // ID remitente
#define DST_ID     6 // ID destinastario

#define LORA_FREQ 915E6

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

uint16_t counter = 0;

void setup() {
  Serial.begin(9600);

  pinMode(RFM_TCX_ON, OUTPUT);
  pinMode(RFM_SWITCH, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(RFM_TCX_ON, HIGH);
  digitalWrite(RFM_SWITCH, HIGH);

  sensors.begin();

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

  Serial.println("Emisor con ID listo");
}

void loop() {
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);

  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("DS18B20 desconectado");
    delay(3000);
    return;
  }

  int16_t temp10 = (int16_t)(tempC * 10.0);

  uint8_t payload[7];

  payload[0] = NETWORK_ID;
  payload[1] = SRC_ID;
  payload[2] = DST_ID;
  payload[3] = highByte(counter);
  payload[4] = lowByte(counter);
  payload[5] = highByte(temp10);
  payload[6] = lowByte(temp10);

  Serial.print("Enviando temp: ");
  Serial.print(tempC, 1);
  Serial.print(" C | Counter: ");
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
