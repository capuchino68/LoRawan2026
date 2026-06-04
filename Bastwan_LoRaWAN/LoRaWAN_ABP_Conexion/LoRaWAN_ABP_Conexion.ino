#include <RadioLib.h>

SX1276 radio = new Module(SS, RFM_DIO0, RFM_RST, RFM_DIO1);

const LoRaWANBand_t Region = US915;
const uint8_t subBand = 1;

LoRaWANNode node(&radio, &Region, subBand);

uint32_t devAddr = 0x260CC911;

uint8_t nwkSKey[] = {
  0x2D, 0x79, 0xBE, 0xBD, 0x7F, 0xC4, 0xF8, 0xDC,
  0x1B, 0x04, 0x3F, 0x51, 0x73, 0x86, 0x5E, 0x20
};

uint8_t appSKey[] = {
  0x2F, 0x67, 0xE9, 0x81, 0x57, 0xAC, 0x6E, 0x87,
  0x88, 0x8E, 0xCE, 0x0B, 0x71, 0xAD, 0xFB, 0xE3
};

unsigned long counter = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  pinMode(RFM_TCX_ON, OUTPUT);
  digitalWrite(RFM_TCX_ON, HIGH);

  Serial.println("BastWAN Counter TTN");

  int state = radio.begin();

  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("Error radio: ");
    Serial.println(state);
    while (true);
  }

  Serial.println("Radio detectado");

  state = node.beginABP(devAddr, NULL, NULL, nwkSKey, appSKey);

  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("Error beginABP: ");
    Serial.println(state);
    while (true);
  }

  state = node.activateABP();

  if (state == RADIOLIB_LORAWAN_NEW_SESSION ||
      state == RADIOLIB_LORAWAN_SESSION_RESTORED ||
      state == RADIOLIB_ERR_NONE) {
    Serial.print("ABP activo. Codigo: ");
    Serial.println(state);
  } else {
    Serial.print("Error activateABP: ");
    Serial.println(state);
    while (true);
  }
}

void loop() {
  char payload[32];

  snprintf(payload, sizeof(payload), "counter-%lu", counter++);

  Serial.print("Enviando: ");
  Serial.println(payload);

  int state = node.sendReceive(
    (uint8_t*)payload,
    strlen(payload),
    1
  );

  Serial.print("Estado envio: ");
  Serial.println(state);

  delay(6000);
}
