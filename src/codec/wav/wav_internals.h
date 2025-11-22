#pragma once

#include "Error.h"
#include "wav.h"

#define WAVE_FORMAT_PCM 0x0001
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#define WAVE_FORMAT_ALAW 0x0006
#define WAVE_FORMAT_MULAW 0x0007
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE

typedef enum {
    Unsigned8,
    Signed16,
    Signed24,
    Signed32,
    Float32,
} SampleFormat;

const char* sampleFormatRepr(SampleFormat fmt);

typedef struct {
    uint8_t nChannels;
    uint32_t sampleRate;
    uint32_t size;
    SampleFormat sampleFormat;
} Header;

Header readWavHeader(FileReader* reader);
void logHeader(const Header* wh);

typedef struct {
    float f;
    Error err;
} FloatResult;

FloatResult readSample(FileReader* reader, SampleFormat fmt);

AudioData readWavData(FileReader* reader, Header h);

#define DUMP_PREFIX "./build/dump_"
#define DUMP_SUFFIX ".csv"

void dumpIntCsv(const int* data, unsigned sz, const char* path);
void dumpFloatCsv(const float* data, unsigned sz, const char* path);
