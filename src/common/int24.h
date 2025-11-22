#pragma once

#include <stdint.h>
#include "DeserializeInts.h"

#define INT32_UNIT ((int32_t)1)
#define INT24_MAX ((INT32_UNIT << 23) - 1)
#define INT24_MIN (-(INT32_UNIT << 23))

typedef struct {
    uint8_t highByte;
    uint8_t middleByte;
    uint8_t lowByte;
} Int24;

Int24 deserializeI24_LE(const Byte* bytes);
int32_t i24_asI32(Int24 i);
