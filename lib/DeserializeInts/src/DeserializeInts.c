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
