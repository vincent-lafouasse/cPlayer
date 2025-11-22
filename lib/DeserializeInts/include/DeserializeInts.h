#pragma once

#include <stdint.h>

typedef uint8_t Byte;

// you better give me 2 bytes or else
uint16_t deserializeU16_LE(const Byte* bytes);
uint16_t deserializeU16_BE(const Byte* bytes);
