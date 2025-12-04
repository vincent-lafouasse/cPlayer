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
} WavHeader;

typedef struct {
    uint32_t size;
    uint16_t formatTag;
    uint16_t nChannels;
    uint32_t sampleRate;
    uint32_t bytesPerSecond;
    uint16_t blockSize;
    uint16_t bitDepth;
    uint16_t extensionSize;
    // extension
    uint16_t validBitsPerSample;
    uint32_t channelMask;
    uint8_t subFormat[16];
} WavFormatChunk;

Error skipChunkUntil(Reader* reader, const char* expectedId);
Error getToFormatChunk(Reader* reader);
Error readFormatChunk(Reader* reader, WavFormatChunk* out);
Error readWavHeader(Reader* reader, WavHeader* out);
void logHeader(const WavHeader* wh);

Error readSample(Reader* reader, SampleFormat fmt, float* out);
AudioDataResult readWavData(Reader* reader, WavHeader h);

#define DUMP_PREFIX "./build/dump_"
#define DUMP_SUFFIX ".csv"

void dumpIntCsv(const int* data, unsigned sz, const char* path);
void dumpFloatCsv(const float* data, unsigned sz, const char* path);
