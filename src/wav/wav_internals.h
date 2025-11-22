#pragma once

#include "wav.h"

#define WAVE_FORMAT_PCM 0x0001
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#define WAVE_FORMAT_ALAW 0x0006
#define WAVE_FORMAT_MULAW 0x0007
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE

typedef struct {
    uint8_t nChannels;
    uint32_t sampleRate;
    uint16_t bitDepth;
    uint32_t size;
    uint32_t runtimeMs;
} Header;

Header readWavHeader(FileReader* reader);
void logHeader(const Header* wh);
AudioData readWavData(FileReader* reader, Header h);

#define DUMP_PREFIX "./build/dump_"
#define DUMP_SUFFIX ".csv"

void dumpIntCsv(const int* data, unsigned sz, const char* path);
void dumpFloatCsv(const float* data, unsigned sz, const char* path);
