#include "int24.h"

typedef uint8_t u8;
typedef Int24 i24;

typedef uint32_t u32;
typedef int32_t i32;

static i32 combine(u8 b3, u8 b2, u8 b1, u8 b0)
{
    u32 byte3 = b3;
    u32 byte2 = b2;
    u32 byte1 = b1;
    u32 byte0 = b0;

    u32 bits = (byte3 << 24) + (byte2 << 16) + (byte1 << 8) + byte0;
    i32* p = (i32*)(&bits);
    return *p;
}

i32 i24_asI32(i24 i)
{
    u8 highByte = i.highByte;
    u8 middleByte = i.middleByte;
    u8 lowByte = i.lowByte;

    // move sign bit
    u8 signByte = highByte & (1 << 7);
    highByte |= (1 << 7);

    return combine(signByte, highByte, middleByte, lowByte);
}
