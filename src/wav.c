#include "wav.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "int24.h"
#include "log.h"

#define WAVE_FORMAT_PCM 0x0001
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#define WAVE_FORMAT_ALAW 0x0006
#define WAVE_FORMAT_MULAW 0x0007
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE

static ReadResult readFourCC(FileReader* reader, uint8_t* out)
{
    for (size_t i = 0; i < 4; ++i) {
        const ReadResult res = fr_takeByte(reader, out + i);
        if (res != Read_Ok) {
            return res;
        }
    }
    return Read_Ok;
}

static ReadResult readI24AsI32LE(FileReader* reader, int32_t* out)
{
    uint8_t bytes[3];
    for (size_t i = 0; i < 3; ++i) {
        const ReadResult res = fr_takeByte(reader, bytes + i);
        if (res != Read_Ok) {
            return res;
        }
    }
    *out = i24_asI32((Int24){bytes[2], bytes[1], bytes[0]});
    return Read_Ok;
}

// unsafe to use with actual i32 but i24 will never reach i32 min/max so it's
// fine
static int32_t maxAbs(int32_t* data, uint32_t sz)
{
    int32_t out = -1;

    for (uint32_t i = 0; i < sz; ++i) {
        int32_t abs = data[i] > 0 ? data[i] : -data[i];
        if (abs > out) {
            out = abs;
        }
    }

    return out;
}

#include <stdio.h>

#define DUMP_PREFIX "./build/dump_"
#define DUMP_SUFFIX ".csv"

void dumpIntCsv(int* data, unsigned sz, const char* path)
{
#if LOGGING
    FILE* dump = fopen(path, "w");

    for (unsigned i = 0; i < sz; ++i) {
        fprintf(dump, "%i,", data[i]);
    }
    fprintf(dump, "\n");

    fclose(dump);
#else
    (void)data;
    (void)sz;
    (void)path;
#endif
}

void dumpFloatCsv(float* data, unsigned sz, const char* path)
{
#if LOGGING
    FILE* dump = fopen(path, "w");

    for (unsigned i = 0; i < sz; ++i) {
        fprintf(dump, "%f,", data[i]);
    }
    fprintf(dump, "\n");

    fclose(dump);
#else
    (void)data;
    (void)sz;
    (void)path;
#endif
}

AudioData readWavData(FileReader* reader, Header h)
{
    int32_t* intData = malloc(h.size * sizeof(int32_t));

    for (uint32_t i = 0; i < h.size; ++i) {
        assert(readI24AsI32LE(reader, intData + i) == Read_Ok);
        uint8_t garbage;
        assert(fr_takeByte(reader, &garbage) == Read_Ok);
        assert(fr_takeByte(reader, &garbage) == Read_Ok);
        assert(fr_takeByte(reader, &garbage) == Read_Ok);
    }
    dumpIntCsv(intData, h.size, DUMP_PREFIX "i24" DUMP_SUFFIX);

    float* data = malloc(h.size * sizeof(float));
    const float normalisationFactor = (float)maxAbs(intData, h.size);
    for (uint32_t i = 0; i < h.size; ++i) {
        data[i] = (float)intData[i] / normalisationFactor;
    }
    dumpFloatCsv(data, h.size, DUMP_PREFIX "float" DUMP_SUFFIX);

    return (AudioData){.h = h, .left = data, .right = data};
}

Header readWavHeader(FileReader* reader)
{
    uint8_t masterChunkID[5] = {0};
    assert(readFourCC(reader, masterChunkID) == Read_Ok);
    assert(strncmp((const char*)masterChunkID, "RIFF", 4) == 0);
    logFn("master chunk ID:\t%s\n", masterChunkID);

    uint32_t masterChunkSize;
    assert(fr_takeU32LE(reader, &masterChunkSize) == Read_Ok);
    logFn("chunk size:\t\t%u bytes\n", masterChunkSize);

    uint8_t wavChunkID[5] = {0};
    assert(readFourCC(reader, wavChunkID) == Read_Ok);
    assert(strncmp((const char*)wavChunkID, "WAVE", 4) == 0);
    logFn("wav chunk ID:\t\t%s\n", wavChunkID);

    uint8_t fmtChunkID[5] = {0};
    assert(readFourCC(reader, fmtChunkID) == Read_Ok);
    assert(strncmp((const char*)fmtChunkID, "fmt ", 4) == 0);
    logFn("fmt chunk ID:\t\t%s\n", fmtChunkID);

    uint32_t fmtChunkSize;
    assert(fr_takeU32LE(reader, &fmtChunkSize) == Read_Ok);
    logFn("format chunk size:\t%u bytes\n", fmtChunkSize);

    uint16_t waveFormat;
    assert(fr_takeU16LE(reader, &waveFormat) == Read_Ok);
    logFn("wave format:\t\t0x%04:x\n", waveFormat);
    if (waveFormat != WAVE_FORMAT_PCM) {
        logFn("\nError:\n\tUnsupported wave format\n");
        logFn("Only PCM is supported for now\n");
        exit(1);
    }

    uint16_t nChannels;
    assert(fr_takeU16LE(reader, &nChannels) == Read_Ok);
    logFn("n. channels:\t\t%x\n", nChannels);

    uint32_t sampleRate;
    assert(fr_takeU32LE(reader, &sampleRate) == Read_Ok);
    logFn("sample rate:\t\t%u\n", sampleRate);

    uint32_t bytesPerSec;
    assert(fr_takeU32LE(reader, &bytesPerSec) == Read_Ok);
    logFn("data rate:\t\t%u bytes per sec\n", bytesPerSec);

    uint16_t blockSize;
    assert(fr_takeU16LE(reader, &blockSize) == Read_Ok);
    logFn("block size:\t\t%u bytes\n", blockSize);

    uint16_t bitDepth;
    assert(fr_takeU16LE(reader, &bitDepth) == Read_Ok);
    logFn("bit depth:\t\t%u bits\n", bitDepth);

    uint16_t formatChunkExtensionSize = fmtChunkSize - 16;
    logFn("fmt extension:\t\t%u bytes\n", formatChunkExtensionSize);
    for (uint16_t i = 0; i < formatChunkExtensionSize; ++i) {
        uint8_t garbage;
        assert(fr_takeByte(reader, &garbage) == Read_Ok);
    }

    uint8_t chunkID[5] = {0};
    assert(readFourCC(reader, chunkID) == Read_Ok);
    logFn("next chunk ID:\t\t%s\n", chunkID);

    uint32_t dataSize;
    assert(fr_takeU32LE(reader, &dataSize) == Read_Ok);
    logFn("data size:\t\t%u\n", dataSize);
    uint32_t nBlocks = 8 * dataSize / bitDepth / nChannels;
    logFn("n blocks:\t\t%u\n", nBlocks);
    float runtime = (float)nBlocks / (float)sampleRate;
    logFn("runtime:\t\t%f secs\n", runtime);

    logFn("\n");
    return (Header){
        .nChannels = nChannels,
        .sampleRate = sampleRate,
        .bitDepth = bitDepth,
        .size = nBlocks,
        .runtimeMs = (uint32_t)(1000.0f * runtime),
    };
}

void logHeader(const Header* wh, const char* name)
{
    const char* resolvedName = name != NULL ? name : "Header";

    logFn("%s {\n", resolvedName);
    logFn("\tnumber of Channels:\t%u\n", wh->nChannels);
    logFn("\tsample rate:\t\t%u Hz\n", wh->sampleRate);
    logFn("\tbit depth:\t\t%u bit\n", wh->bitDepth);
    logFn("\tnumber of samples:\t%u\n", wh->size);
    logFn("}\n\n");
}
