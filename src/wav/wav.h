#pragma once

#include <stdint.h>

#include "FileReader.h"

typedef struct {
    uint8_t nChannels;
    uint32_t sampleRate;
    uint16_t bitDepth;
    uint32_t size;
    uint32_t runtimeMs;
} Header;

Header readWavHeader(FileReader* reader);
void logHeader(const Header* wh, const char* name);

typedef struct {
    Header h;
    float* left;
    float* right;
} AudioData;

AudioData readWavData(FileReader* reader, Header h);
