#include "bitcast.h"

uint16_t bitcastU16_LE(const Byte* bytes)
{
	const uint16_t low = bytes[0];
	const uint16_t high = bytes[1];
	return (high << 8) | low;
}

uint16_t bitcastU16_BE(const Byte* bytes)
{
	const uint16_t high = bytes[0];
	const uint16_t low = bytes[1];
	return (high << 8) | low;
}

uint32_t bitcastU32_LE(const Byte* bytes)
{
	const uint32_t low = bytes[0];
	const uint32_t lowish = bytes[1];
	const uint32_t highish = bytes[2];
	const uint32_t high = bytes[3];
	return (high << 24) | (highish << 16) | (lowish << 8) | low;
}

uint32_t bitcastU32_BE(const Byte* bytes)
{
	const uint32_t high = bytes[0];
	const uint32_t highish = bytes[1];
	const uint32_t lowish = bytes[2];
	const uint32_t low = bytes[3];
	return (high << 24) | (highish << 16) | (lowish << 8) | low;
}

int16_t bitcastI16_LE(const Byte* bytes)
{
	uint16_t value = bitcastU16_LE(bytes);
	return *(int16_t*)&value;
}

int16_t bitcastI16_BE(const Byte* bytes)
{
	uint16_t value = bitcastU16_BE(bytes);
	return *(int16_t*)&value;
}

int32_t bitcastI32_LE(const Byte* bytes)
{
	uint32_t value = bitcastU32_LE(bytes);
	return *(int32_t*)&value;
}

int32_t bitcastI32_BE(const Byte* bytes)
{
	uint32_t value = bitcastU32_BE(bytes);
	return *(int32_t*)&value;
}
