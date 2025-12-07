#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef uint8_t Byte;

uint16_t bitcastU16_LE(const Byte bytes[2]);
uint16_t bitcastU16_BE(const Byte bytes[2]);
int16_t bitcastI16_LE(const Byte bytes[2]);
int16_t bitcastI16_BE(const Byte bytes[2]);

uint32_t bitcastU24_LE(const Byte bytes[3]);
uint32_t bitcastU24_BE(const Byte bytes[3]);
int32_t bitcastI24_LE(const Byte bytes[3]);
int32_t bitcastI24_BE(const Byte bytes[3]);

uint32_t bitcastU32_LE(const Byte bytes[4]);
uint32_t bitcastU32_BE(const Byte bytes[4]);
int32_t bitcastI32_LE(const Byte bytes[4]);
int32_t bitcastI32_BE(const Byte bytes[4]);

uint64_t bitcastU64_LE(const Byte bytes[8]);
uint64_t bitcastU64_BE(const Byte bytes[8]);
int64_t bitcastI64_LE(const Byte bytes[8]);
int64_t bitcastI64_BE(const Byte bytes[8]);

#ifdef __cplusplus
}
#endif
