#pragma once

#include <stdint.h>

typedef struct {
    uint8_t highByte;
    uint8_t middleByte;
    uint8_t lowByte;
} Int24;

int32_t i24_asI32(Int24 i);
