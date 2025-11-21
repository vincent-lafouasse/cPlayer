#pragma once

#include <stdint.h>

#include "FileReader.h"
#include "audio.h"

typedef struct {
    uint8_t nChannels;
    uint32_t sampleRate;
    uint16_t bitDepth;
    uint32_t size;
    uint32_t runtimeMs;
} Header;

Header readWavHeader(FileReader* reader);
void logHeader(const Header* wh, const char* name);

AudioData readWavData(FileReader* reader, Header h);
