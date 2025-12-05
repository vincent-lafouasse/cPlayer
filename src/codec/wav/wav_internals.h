#pragma once

#include "Error.h"
#include "wav.h"

#define WAVE_FORMAT_PCM 0x0001
#define WAVE_FORMAT_ADPCM 0x0002
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#define WAVE_FORMAT_ALAW 0x0006
#define WAVE_FORMAT_MULAW 0x0007
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE

typedef struct {
    uint32_t size;
    uint16_t formatTag;
    uint16_t nChannels;
    uint32_t sampleRate;
    uint32_t bytesPerSecond;
    uint16_t blockAlign;
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

typedef enum {
    SampleFormat_Unsigned8,
    SampleFormat_Signed16,
    SampleFormat_Signed24,
    SampleFormat_Signed32,
    SampleFormat_Float32,
    SampleFormat_Float64,
    SampleFormat_ADPCM,  // for IMA-ADPCM blocks
    SampleFormat_MULAW,  // μ-law
    SampleFormat_ALAW,   // A-law
} SampleFormat;

const char* sampleFormatRepr(SampleFormat fmt);

typedef struct {
    uint16_t formatTag;
    uint16_t nChannels;
    uint32_t sampleRate;
    SampleFormat sampleFormat;
    uint16_t blockAlign;  // bytes per sample block (nChannels * bytesPerSample
                          // or ADPCM block)
    uint32_t nFrames;
    uint16_t bitsPerSample;
    uint16_t adpcmBlockSize;  // only for ADPCM, else 0
} WavFormatInfo;

Error validateWavFormatChunk(const WavFormatChunk* format, SampleFormat* out);
Error readWavFormatInfo(Reader* reader, WavFormatInfo* out);
void logWavFormatInfo(const WavFormatInfo* format);

Error readSample(Reader* reader, const WavFormatInfo* format, float* out);
AudioDataResult readWavData(Reader* reader, const WavFormatInfo* format);

#define DUMP_PREFIX "./build/dump_"
#define DUMP_SUFFIX ".csv"

void dumpIntCsv(const int* data, unsigned sz, const char* path);
void dumpFloatCsv(const float* data, unsigned sz, const char* path);
