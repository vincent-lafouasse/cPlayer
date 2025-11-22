#pragma once

#include <stdint.h>

typedef uint8_t Byte;

// you better give me 2 bytes or else
uint16_t deserializeU16_LE(const Byte* bytes);
uint16_t deserializeU16_BE(const Byte* bytes);


// you better give me 4 bytes or else
uint32_t deserializeU32_LE(const Byte* bytes);
uint32_t deserializeU32_BE(const Byte* bytes);
