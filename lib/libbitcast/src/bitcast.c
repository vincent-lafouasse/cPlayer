#include "bitcast.h"

uint16_t bitcastU16_LE(const Byte bytes[2])
{
	return ((uint16_t)bytes[1] << 8) | (uint16_t)bytes[0];
}

uint16_t bitcastU16_BE(const Byte bytes[2])
{
	return ((uint16_t)bytes[0] << 8) | (uint16_t)bytes[1];
}

uint32_t bitcastU32_LE(const Byte bytes[4])
{
	return ((uint32_t)bytes[3] << 24) | ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[1] << 8) | (uint32_t)bytes[0];
}

uint32_t bitcastU32_BE(const Byte bytes[4])
{
	return ((uint32_t)bytes[0] << 24) | ((uint32_t)bytes[1] << 16) | ((uint32_t)bytes[2] << 8) | (uint32_t)bytes[3];
}

int16_t bitcastI16_LE(const Byte bytes[2])
{
	return  (int16_t)bitcastU16_LE(bytes);
}

int16_t bitcastI16_BE(const Byte bytes[2])
{
	return  (int16_t)bitcastU16_BE(bytes);
}

int32_t bitcastI32_LE(const Byte bytes[4])
{
	return  (int32_t)bitcastU32_LE(bytes);
}

int32_t bitcastI32_BE(const Byte bytes[4])
{
	return  (int32_t)bitcastU32_BE(bytes);
}
