//ElectronicCats
//Envío de un counter para prueba de conexión a Gateway

#include <RadioLib.h>

SX1276 radio = new Module(SS, RFM_DIO0, RFM_RST, RFM_DIO1);

const LoRaWANBand_t Region = US915;
const uint8_t subBand = 2;

LoRaWANNode node(&radio, &Region, subBand);

uint32_t devAddr = 0x260C38FB;

uint8_t nwkSKey[] = {
  0x5C, 0x85, 0x4B, 0x38, 0x1C, 0x82, 0x56, 0x84,
  0x20, 0x36, 0xFE, 0xB8, 0x1D, 0x0B, 0xBF, 0xFC
};

uint8_t appSKey[] = {
 0x4A, 0x1F, 0xAA, 0x60, 0xF9, 0xD8, 0x50, 0x75,
 0x0E, 0x11, 0xDF, 0xF3, 0x31, 0x75, 0x47, 0xC1
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
