#pragma once

#include <stdint.h>

#include "FileReader.h"

typedef struct {
    uint8_t nChannels;
    uint32_t sampleRate;
    uint16_t bitDepth;
    uint32_t size;
} WavHeader;

WavHeader readWavHeader(FileReader* reader);
void logWavHeader(const WavHeader* wh, const char* name);
