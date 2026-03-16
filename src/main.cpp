#include "./main.hpp"

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial && millis() < 2000);

  if (!CAN.begin(CAN_BAUD_RATE)) { Serial.println("CAN.begin failed"); while (1); }

  Serial.println("CAN init OK");
}

void loop() {
  static uint32_t lastSample = 0;
  uint32_t now = millis();
  if (now - lastSample < SAMPLE_INTERVAL_MS) return;
  lastSample = now;

  uint16_t v[6];
  for (int i = 0; i < 6; i++) v[i] = (uint16_t)analogRead(A0 + i);

  uint8_t d[8];
  d[0] = v[0] >> 2;
  d[1] = ((v[0] & 0x3) << 6) | (v[1] >> 4);
  d[2] = ((v[1] & 0xF) << 4) | (v[2] >> 6);
  d[3] = ((v[2] & 0x3F) << 2) | (v[3] >> 8);
  d[4] = v[3] & 0xFF;
  d[5] = v[4] >> 2;
  d[6] = ((v[4] & 0x3) << 6) | (v[5] >> 4);
  d[7] = (v[5] & 0xF) << 4;

  CanMsg frame(CanStandardId(CAN_ID_FRAME), sizeof(d), d);
  CAN.write(frame);
}
