#include "bitcast.h"

uint16_t bitcastU16_LE(const Byte bytes[2])
{
    return ((uint16_t)bytes[1] << 8) | (uint16_t)bytes[0];
}

uint16_t bitcastU16_BE(const Byte bytes[2])
{
    return ((uint16_t)bytes[0] << 8) | (uint16_t)bytes[1];
}

uint32_t bitcastU24_LE(const Byte bytes[3])
{
    return ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[1] << 8) |
           (uint32_t)bytes[0];
}

uint32_t bitcastU24_BE(const Byte bytes[3])
{
    return ((uint32_t)bytes[0] << 16) | ((uint32_t)bytes[1] << 8) |
           (uint32_t)bytes[2];
}

uint32_t bitcastU32_LE(const Byte bytes[4])
{
    return ((uint32_t)bytes[3] << 24) | ((uint32_t)bytes[2] << 16) |
           ((uint32_t)bytes[1] << 8) | (uint32_t)bytes[0];
}

uint32_t bitcastU32_BE(const Byte bytes[4])
{
    return ((uint32_t)bytes[0] << 24) | ((uint32_t)bytes[1] << 16) |
           ((uint32_t)bytes[2] << 8) | (uint32_t)bytes[3];
}

uint64_t bitcastU64_LE(const Byte bytes[8])
{
    return ((uint64_t)bytes[7] << 56) | ((uint64_t)bytes[6] << 48) |
           ((uint64_t)bytes[5] << 40) | ((uint64_t)bytes[4] << 32) |
           ((uint64_t)bytes[3] << 24) | ((uint64_t)bytes[2] << 16) |
           ((uint64_t)bytes[1] << 8) | ((uint64_t)bytes[0]);
}

uint64_t bitcastU64_BE(const Byte bytes[8])
{
    return ((uint64_t)bytes[0] << 56) | ((uint64_t)bytes[1] << 48) |
           ((uint64_t)bytes[2] << 40) | ((uint64_t)bytes[3] << 32) |
           ((uint64_t)bytes[4] << 24) | ((uint64_t)bytes[5] << 16) |
           ((uint64_t)bytes[6] << 8) | ((uint64_t)bytes[7]);
}

int16_t bitcastI16_LE(const Byte bytes[2])
{
    return (int16_t)bitcastU16_LE(bytes);
}

int16_t bitcastI16_BE(const Byte bytes[2])
{
    return (int16_t)bitcastU16_BE(bytes);
}

int32_t bitcastI24_LE(const Byte bytes[3])
{
    uint32_t value = bitcastU24_LE(bytes);

    // extend sign bit
    if (value & 0x00800000)
        value |= 0xFF000000;

    return (int32_t)value;
}

int32_t bitcastI24_BE(const Byte bytes[3])
{
    uint32_t value = bitcastU24_BE(bytes);

    // extend sign bit
    if (value & 0x00800000)
        value |= 0xFF000000;

    return (int32_t)value;
}

int32_t bitcastI32_LE(const Byte bytes[4])
{
    return (int32_t)bitcastU32_LE(bytes);
}

int32_t bitcastI32_BE(const Byte bytes[4])
{
    return (int32_t)bitcastU32_BE(bytes);
}

int64_t bitcastI64_LE(const Byte bytes[8])
{
    return (int64_t)bitcastU64_LE(bytes);
}

int64_t bitcastI64_BE(const Byte bytes[8])
{
    return (int64_t)bitcastU64_BE(bytes);
}
