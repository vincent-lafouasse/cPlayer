#include "wav_internals.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

Header readWavHeader(FileReader* reader)
{
    uint8_t masterChunkID[5] = {0};
    assert(readFourCC(reader, masterChunkID) == Read_Ok);
    assert(strncmp((const char*)masterChunkID, "RIFF", 4) == 0);
    logFn(Debug, "master chunk ID:\t%s\n", masterChunkID);

    uint32_t masterChunkSize;
    assert(fr_takeU32LE(reader, &masterChunkSize) == Read_Ok);
    logFn(Debug, "chunk size:\t\t%u bytes\n", masterChunkSize);

    uint8_t wavChunkID[5] = {0};
    assert(readFourCC(reader, wavChunkID) == Read_Ok);
    assert(strncmp((const char*)wavChunkID, "WAVE", 4) == 0);
    logFn(Debug, "wav chunk ID:\t\t%s\n", wavChunkID);

    uint8_t fmtChunkID[5] = {0};
    assert(readFourCC(reader, fmtChunkID) == Read_Ok);
    assert(strncmp((const char*)fmtChunkID, "fmt ", 4) == 0);
    logFn(Debug, "fmt chunk ID:\t\t%s\n", fmtChunkID);

    uint32_t fmtChunkSize;
    assert(fr_takeU32LE(reader, &fmtChunkSize) == Read_Ok);
    logFn(Debug, "format chunk size:\t%u bytes\n", fmtChunkSize);

    uint16_t waveFormat;
    assert(fr_takeU16LE(reader, &waveFormat) == Read_Ok);
    logFn(Debug, "wave format:\t\t0x%04:x\n", waveFormat);
    if (waveFormat != WAVE_FORMAT_PCM) {
        logFn(Error, "\nError:\n\tUnsupported wave format\n");
        logFn(Error, "Only PCM is supported for now\n");
        exit(1);
    }

    uint16_t nChannels;
    assert(fr_takeU16LE(reader, &nChannels) == Read_Ok);
    logFn(Debug, "n. channels:\t\t%x\n", nChannels);

    uint32_t sampleRate;
    assert(fr_takeU32LE(reader, &sampleRate) == Read_Ok);
    logFn(Debug, "sample rate:\t\t%u\n", sampleRate);

    uint32_t bytesPerSec;
    assert(fr_takeU32LE(reader, &bytesPerSec) == Read_Ok);
    logFn(Debug, "data rate:\t\t%u bytes per sec\n", bytesPerSec);

    uint16_t blockSize;
    assert(fr_takeU16LE(reader, &blockSize) == Read_Ok);
    logFn(Debug, "block size:\t\t%u bytes\n", blockSize);

    uint16_t bitDepth;
    assert(fr_takeU16LE(reader, &bitDepth) == Read_Ok);
    logFn(Debug, "bit depth:\t\t%u bits\n", bitDepth);

    uint16_t formatChunkExtensionSize = fmtChunkSize - 16;
    logFn(Debug, "fmt extension:\t\t%u bytes\n", formatChunkExtensionSize);
    for (uint16_t i = 0; i < formatChunkExtensionSize; ++i) {
        uint8_t garbage;
        assert(fr_takeByte(reader, &garbage) == Read_Ok);
    }

    uint8_t chunkID[5] = {0};
    assert(readFourCC(reader, chunkID) == Read_Ok);
    logFn(Debug, "next chunk ID:\t\t%s\n", chunkID);

    uint32_t dataSize;
    assert(fr_takeU32LE(reader, &dataSize) == Read_Ok);
    logFn(Debug, "data size:\t\t%u\n", dataSize);
    uint32_t nBlocks = 8 * dataSize / bitDepth / nChannels;
    logFn(Debug, "n blocks:\t\t%u\n", nBlocks);
    float runtime = (float)nBlocks / (float)sampleRate;
    logFn(Debug, "runtime:\t\t%f secs\n", runtime);

    logFn(Debug, "\n");
    return (Header){
        .nChannels = nChannels,
        .sampleRate = sampleRate,
        .bitDepth = bitDepth,
        .size = nBlocks,
        .runtimeMs = (uint32_t)(1000.0f * runtime),
    };
}

void logHeader(const Header* wh)
{
    logFn(Info, "%s {\n", "WavHeader");
    logFn(Info, "\tnumber of Channels:\t%u\n", wh->nChannels);
    logFn(Info, "\tsample rate:\t\t%u Hz\n", wh->sampleRate);
    logFn(Info, "\tbit depth:\t\t%u bit\n", wh->bitDepth);
    logFn(Info, "\tnumber of samples:\t%u\n", wh->size);
    logFn(Info, "}\n\n");
}
