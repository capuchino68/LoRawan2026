//by Edgar Capuchino
//Electronic Cats
//Conexión a Gateway y en vio de datos de sensor DS18B20 con CayenneLPP
#include <RadioLib.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <CayenneLPP.h>

#define ONE_WIRE_BUS 5

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

CayenneLPP lpp(51);

SX1276 radio = new Module(SS, RFM_DIO0, RFM_RST, RFM_DIO1);

const LoRaWANBand_t Region = US915;
const uint8_t subBand = 1;

LoRaWANNode node(&radio, &Region, subBand);

uint32_t devAddr = 0x260C366B;

uint8_t nwkSKey[] = {
  0x49, 0x98, 0x85, 0xC2, 0xD8, 0xCB, 0xF1, 0x82, 
  0xCA, 0x5A, 0xBF, 0xCE, 0x6B, 0xCB, 0x71, 0xC4
};

uint8_t appSKey[] = {
  0x8B, 0xC7, 0x6C, 0x08, 0xA0, 0x6D, 0xA1, 0x1E, 
  0x86, 0x4A, 0x3E, 0xBC, 0x20, 0x28, 0xBD, 0x0C
};

void setup() {
  Serial.begin(115200);
  while (!Serial);

  pinMode(RFM_TCX_ON, OUTPUT);
  digitalWrite(RFM_TCX_ON, HIGH);

  sensors.begin();

  Serial.println("BastWAN + RadioLib + DS18B20 + CayenneLPP");

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
  sensors.requestTemperatures();

  float t = sensors.getTempCByIndex(0);

  if (t == DEVICE_DISCONNECTED_C) {
    Serial.println("ERROR: DS18B20 desconectado");
    delay(10000);
    return;
  }

  Serial.print("Temperatura C: ");
  Serial.println(t, 2);

  lpp.reset();

  // Canal 1 = temperatura DS18B20
  lpp.addTemperature(1, t);

  Serial.print("Payload CayenneLPP HEX: ");
  for (uint8_t i = 0; i < lpp.getSize(); i++) {
    if (lpp.getBuffer()[i] < 0x10) Serial.print("0");
    Serial.print(lpp.getBuffer()[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  int state = node.sendReceive(
    lpp.getBuffer(),
    lpp.getSize(),
    1
  );

  Serial.print("Estado envio: ");
  Serial.println(state);

  delay(6000);
}
