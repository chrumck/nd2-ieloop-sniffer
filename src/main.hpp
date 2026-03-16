#include <Arduino.h>
#include <Arduino_CAN.h>

#define SERIAL_BAUD_RATE 115200
#define CAN_BAUD_RATE CanBitRate::BR_500k

constexpr uint32_t CAN_ID_FRAME       = 0x7F0;
constexpr uint32_t SAMPLE_INTERVAL_MS = 20;