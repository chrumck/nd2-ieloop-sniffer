#include "./main.hpp"

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial);

  if (!CAN.begin(CAN_BAUD_RATE)) { Serial.println("CAN.begin failed"); while (1); }

  Serial.println("CAN init OK");
}

void loop() {
}
