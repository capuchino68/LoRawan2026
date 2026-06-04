#include <SPI.h>
#include <LoRa.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 5

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);

  pinMode(RFM_TCX_ON, OUTPUT);
  pinMode(RFM_SWITCH, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(RFM_TCX_ON, HIGH);

  sensors.begin();

  LoRa.setPins(SS, RFM_RST, RFM_DIO0);

  if (!LoRa.begin(915E6)) {
    Serial.println("LoRa failed");
    while (1);
  }

  Serial.println("LoRa Sender OK");
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);

  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);

  String message = String(tempC, 1);

  Serial.print("Sending temp: ");
  Serial.println(message);

  digitalWrite(RFM_SWITCH, HIGH);

  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();

  digitalWrite(LED_BUILTIN, LOW);

  delay(5000);
}
