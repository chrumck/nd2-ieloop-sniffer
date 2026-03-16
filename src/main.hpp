#include <Arduino.h>
#include <Arduino_CAN.h>

#define SERIAL_BAUD_RATE 115200
#define CAN_BAUD_RATE CanBitRate::BR_500k

constexpr uint32_t CAN_ID_FRAME       = 0x7F0;
constexpr uint32_t SAMPLE_INTERVAL_MS = 20;

constexpr float ADC_VREF        = 3.3f;
constexpr float ADC_FULL_SCALE  = 16383.0f;
constexpr float SCALE_CH0_3     = 7.91f;
constexpr float SCALE_CH4_5     = 11.0f;
constexpr float COUNTS_PER_VOLT = 1000.0f / 25.0f;