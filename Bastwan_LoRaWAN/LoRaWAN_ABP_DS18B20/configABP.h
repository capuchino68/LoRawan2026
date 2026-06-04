#include <RadioLib.h>

// Pines tipo BastWAN/Beelan.
// Si alguno no compila, hay que revisar pins_arduino.h de la placa.
SX1276 radio = new Module(SS, RFM_DIO0, RFM_RST, RFM_DIO1);

void setup() {
  Serial.begin(115200);
  while (!Serial);

  pinMode(RFM_TCX_ON, OUTPUT);
  digitalWrite(RFM_TCX_ON, HIGH);

  Serial.println("Probando RadioLib en BastWAN...");

  int state = radio.begin(904.1);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("RadioLib detecto el radio correctamente.");
  } else {
    Serial.print("Error al iniciar radio. Codigo: ");
    Serial.println(state);
  }
}

void loop() {}
