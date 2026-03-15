#include <Arduino.h>
#include <Arduino_CAN.h>

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!CAN.begin(CanBitRate::BR_500k)) {
    Serial.println("CAN.begin failed");
    while (1);
  }

  Serial.println("CAN init OK");
}

void loop() {
}
