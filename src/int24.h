#pragma once

#include <stdint.h>

typedef struct {
    uint8_t lowByte;
    uint8_t middleByte;
    uint8_t highByte;
} Int24;

int32_t i24(Int24 i);
