#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef uint8_t Byte;

// you better give me 2 bytes or else
uint16_t	bitcastU16_LE(const Byte* bytes);
uint16_t	bitcastU16_BE(const Byte* bytes);
int16_t		bitcastI16_LE(const Byte* bytes);
int16_t		bitcastI16_BE(const Byte* bytes);


// you better give me 4 bytes or else
uint32_t	bitcastU32_LE(const Byte* bytes);
uint32_t	bitcastU32_BE(const Byte* bytes);
int32_t		bitcastI32_LE(const Byte* bytes);
int32_t		bitcastI32_BE(const Byte* bytes);

#ifdef __cplusplus
}
#endif
