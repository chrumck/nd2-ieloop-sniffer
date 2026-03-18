#include <Arduino.h>
#include <Arduino_CAN.h>

constexpr uint32_t SERIAL_BAUD_RATE = 115200;
constexpr CanBitRate CAN_BAUD_RATE = CanBitRate::BR_500k;

constexpr uint32_t CAN_ID_FRAME = 0x7F0;
constexpr uint32_t SAMPLE_INTERVAL_MS = 20;

constexpr int   ADC_CHANNEL_COUNT = 6;
constexpr float ADC_VREF = 5.0f;
constexpr float ADC_FULL_SCALE = 16383.0f;
constexpr float CH_SCALE[ADC_CHANNEL_COUNT] = { 7.65f, 7.47f, 7.69f, 7.78f, 10.91f, 10.98f };
constexpr float CH_OFFSET[ADC_CHANNEL_COUNT] = { 0.0875f,  0.0875f,  0.0875f,  0.0875f,  0.0125f,  0.0125f };
constexpr float COUNTS_PER_VOLT = 1000.0f / 25.0f;