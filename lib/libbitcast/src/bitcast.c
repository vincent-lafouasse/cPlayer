#include "bitcast.h"

uint16_t bitcastU16_LE(const Byte bytes[2])
{
	const uint16_t low = bytes[0];
	const uint16_t high = bytes[1];
	return (high << 8) | low;
}

uint16_t bitcastU16_BE(const Byte bytes[2])
{
	const uint16_t high = bytes[0];
	const uint16_t low = bytes[1];
	return (high << 8) | low;
}

uint32_t bitcastU32_LE(const Byte bytes[4])
{
	const uint32_t low = bytes[0];
	const uint32_t lowish = bytes[1];
	const uint32_t highish = bytes[2];
	const uint32_t high = bytes[3];
	return (high << 24) | (highish << 16) | (lowish << 8) | low;
}

uint32_t bitcastU32_BE(const Byte bytes[4])
{
	const uint32_t high = bytes[0];
	const uint32_t highish = bytes[1];
	const uint32_t lowish = bytes[2];
	const uint32_t low = bytes[3];
	return (high << 24) | (highish << 16) | (lowish << 8) | low;
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
