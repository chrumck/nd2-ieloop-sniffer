#include <Arduino.h>
#include <Arduino_CAN.h>

constexpr uint32_t SERIAL_BAUD_RATE = 115200;
constexpr CanBitRate CAN_BAUD_RATE = CanBitRate::BR_500k;

constexpr uint32_t CAN_ID_FRAME = 0x7F0;
constexpr uint32_t SAMPLE_INTERVAL_MS = 20;

constexpr float ADC_VREF = 5.0f;
constexpr float ADC_FULL_SCALE = 16383.0f;
constexpr float CH_SCALE[6]  = { 7.55f, 7.55f, 7.55f, 7.55f, 11.0f, 11.0f };
constexpr float CH_OFFSET[6] = { 0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f  };
constexpr float COUNTS_PER_VOLT = 1000.0f / 25.0f;