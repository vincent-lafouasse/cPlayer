#include "DeserializeInts.h"
#include "FileReader.h"
#include "wav_internals.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

Header readWavHeader(FileReader* reader)
{
    uint8_t buffer[5] = {0};

    assert(fr_takeSlice(reader, buffer, 4) == Read_Ok);
    assert(strncmp((const char*)buffer, "RIFF", 4) == 0);
    logFn(Debug, "master chunk ID:\t%s\n", buffer);

    assert(fr_takeSlice(reader, buffer, 4) == Read_Ok);
    const uint32_t masterChunkSize = deserializeU32_LE(buffer);
    logFn(Debug, "chunk size:\t\t%u bytes\n", masterChunkSize);

    assert(fr_takeSlice(reader, buffer, 4) == Read_Ok);
    assert(strncmp((const char*)buffer, "WAVE", 4) == 0);
    logFn(Debug, "wav chunk ID:\t\t%s\n", buffer);

    assert(fr_takeSlice(reader, buffer, 4) == Read_Ok);
    assert(strncmp((const char*)buffer, "fmt ", 4) == 0);
    logFn(Debug, "fmt chunk ID:\t\t%s\n", buffer);

    assert(fr_takeSlice(reader, buffer, 4) == Read_Ok);
    const uint32_t fmtChunkSize = deserializeU32_LE(buffer);
    logFn(Debug, "format chunk size:\t%u bytes\n", fmtChunkSize);

    assert(fr_takeSlice(reader, buffer, 2) == Read_Ok);
    const uint16_t waveFormat = deserializeU16_LE(buffer);
    logFn(Debug, "wave format:\t\t0x%04:x\n", waveFormat);
    if (waveFormat != WAVE_FORMAT_PCM) {
        logFn(Error, "\nError:\n\tUnsupported wave format\n");
        logFn(Error, "Only PCM is supported for now\n");
        exit(1);
    }

    assert(fr_takeSlice(reader, buffer, 2) == Read_Ok);
    const uint32_t nChannels = deserializeU32_LE(buffer);
    logFn(Debug, "n. channels:\t\t%x\n", nChannels);

    assert(fr_takeSlice(reader, buffer, 4) == Read_Ok);
    const uint32_t sampleRate = deserializeU32_LE(buffer);
    logFn(Debug, "sample rate:\t\t%u\n", sampleRate);

    assert(fr_takeSlice(reader, buffer, 4) == Read_Ok);
    const uint32_t bytesPerSec = deserializeU32_LE(buffer);
    logFn(Debug, "data rate:\t\t%u bytes per sec\n", bytesPerSec);

    assert(fr_takeSlice(reader, buffer, 2) == Read_Ok);
    const uint16_t blockSize = deserializeU16_LE(buffer);
    logFn(Debug, "block size:\t\t%u bytes\n", blockSize);

    assert(fr_takeSlice(reader, buffer, 2) == Read_Ok);
    const uint16_t bitDepth = deserializeU16_LE(buffer);
    logFn(Debug, "bit depth:\t\t%u bits\n", bitDepth);

    uint16_t formatChunkExtensionSize = fmtChunkSize - 16;
    logFn(Debug, "fmt extension:\t\t%u bytes\n", formatChunkExtensionSize);
    for (uint16_t i = 0; i < formatChunkExtensionSize; ++i) {
        uint8_t garbage;
        assert(fr_takeByte(reader, &garbage) == Read_Ok);
    }

    assert(fr_takeSlice(reader, buffer, 4) == Read_Ok);
    logFn(Debug, "next chunk ID:\t\t%s\n", buffer);

    assert(fr_takeSlice(reader, buffer, 4) == Read_Ok);
    uint32_t dataSize = deserializeU32_LE(buffer);
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
