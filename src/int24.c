#include "int24.h"

int32_t i24_asI32(Int24 i)
{
    int32_t highByte = i.highByte;
    int32_t middleByte = i.middleByte;
    int32_t lowByte = i.lowByte;
    return (lowByte) + (middleByte << 8) + (highByte << 16);
}
