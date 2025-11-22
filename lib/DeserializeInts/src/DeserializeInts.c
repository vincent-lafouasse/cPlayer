#include "DeserializeInts.h"

uint16_t deserializeU16_LE(const Byte* bytes)
{
	const uint16_t low = bytes[0];
	const uint16_t high = bytes[1];
	return (high << 8) | low;
}

uint16_t deserializeU16_BE(const Byte* bytes)
{
	const uint16_t high = bytes[0];
	const uint16_t low = bytes[1];
	return (high << 8) | low;
}

uint32_t deserializeU32_LE(const Byte* bytes)
{
	const uint32_t low = bytes[0];
	const uint32_t lowish = bytes[1];
	const uint32_t highish = bytes[2];
	const uint32_t high = bytes[3];
	return (high << 24) | (highish << 16) | (lowish << 8) | low;
}

uint32_t deserializeU32_BE(const Byte* bytes)
{
	const uint32_t high = bytes[0];
	const uint32_t highish = bytes[1];
	const uint32_t lowish = bytes[2];
	const uint32_t low = bytes[3];
	return (high << 24) | (highish << 16) | (lowish << 8) | low;
}

int16_t deserializeI16_LE(const Byte* bytes)
{
	uint16_t value = deserializeU16_LE(bytes);
    return *(int16_t*)&value;
}

int16_t deserializeI16_BE(const Byte* bytes)
{
	uint16_t value = deserializeU16_BE(bytes);
    return *(int16_t*)&value;
}
