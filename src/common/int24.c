#include "int24.h"

int32_t i24_asI32(Int24 i)
{
    int32_t value = (i.highByte << 16) | (i.middleByte << 8) | i.lowByte;

    // extend sign bit
    if (value & 0x00800000)
        value |= 0xFF000000;

    return value;
}
