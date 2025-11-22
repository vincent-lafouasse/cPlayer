#include "FileReader.h"
#include "bitcast.h"
#include "wav_internals.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

Header readWavHeader(FileReader* reader)
{
    SliceResult maybeSlice = fr_takeSlice(reader, 4);
    assert(maybeSlice.err == Read_Ok);
    assert(strncmp((const char*)maybeSlice.slice, "RIFF", 4) == 0);
    logFn(Debug, "master chunk ID:\t%s\n", maybeSlice.slice);

    maybeSlice = fr_takeSlice(reader, 4);
    assert(maybeSlice.err == Read_Ok);
    const uint32_t masterChunkSize = bitcastU32_LE(maybeSlice.slice);
    logFn(Debug, "chunk size:\t\t%u bytes\n", masterChunkSize);

    maybeSlice = fr_takeSlice(reader, 4);
    assert(maybeSlice.err == Read_Ok);
    assert(strncmp((const char*)maybeSlice.slice, "WAVE", 4) == 0);
    logFn(Debug, "wav chunk ID:\t\t%s\n", maybeSlice.slice);

    maybeSlice = fr_takeSlice(reader, 4);
    assert(maybeSlice.err == Read_Ok);
    assert(strncmp((const char*)maybeSlice.slice, "fmt ", 4) == 0);
    logFn(Debug, "fmt chunk ID:\t\t%s\n", maybeSlice.slice);

    maybeSlice = fr_takeSlice(reader, 4);
    assert(maybeSlice.err == Read_Ok);
    const uint32_t fmtChunkSize = bitcastU32_LE(maybeSlice.slice);
    logFn(Debug, "format chunk size:\t%u bytes\n", fmtChunkSize);

    maybeSlice = fr_takeSlice(reader, 2);
    assert(maybeSlice.err == Read_Ok);
    const uint16_t waveFormat = bitcastU16_LE(maybeSlice.slice);
    logFn(Debug, "wave format:\t\t0x%04:x\n", waveFormat);
    if (waveFormat != WAVE_FORMAT_PCM) {
        logFn(Error, "\nError:\n\tUnsupported wave format\n");
        logFn(Error, "Only PCM is supported for now\n");
        exit(1);
    }

    maybeSlice = fr_takeSlice(reader, 2);
    assert(maybeSlice.err == Read_Ok);
    const uint32_t nChannels = bitcastU16_LE(maybeSlice.slice);
    logFn(Debug, "n. channels:\t\t%x\n", nChannels);

    maybeSlice = fr_takeSlice(reader, 4);
    assert(maybeSlice.err == Read_Ok);
    const uint32_t sampleRate = bitcastU32_LE(maybeSlice.slice);
    logFn(Debug, "sample rate:\t\t%u\n", sampleRate);

    maybeSlice = fr_takeSlice(reader, 4);
    assert(maybeSlice.err == Read_Ok);
    const uint32_t bytesPerSec = bitcastU32_LE(maybeSlice.slice);
    logFn(Debug, "data rate:\t\t%u bytes per sec\n", bytesPerSec);

    maybeSlice = fr_takeSlice(reader, 2);
    assert(maybeSlice.err == Read_Ok);
    const uint16_t blockSize = bitcastU16_LE(maybeSlice.slice);
    logFn(Debug, "block size:\t\t%u bytes\n", blockSize);

    maybeSlice = fr_takeSlice(reader, 2);
    assert(maybeSlice.err == Read_Ok);
    const uint16_t bitDepth = bitcastU16_LE(maybeSlice.slice);
    logFn(Debug, "bit depth:\t\t%u bits\n", bitDepth);

    uint16_t formatChunkExtensionSize = fmtChunkSize - 16;
    logFn(Debug, "fmt extension:\t\t%u bytes\n", formatChunkExtensionSize);
    assert(fr_skip(reader, formatChunkExtensionSize) == Read_Ok);

    maybeSlice = fr_takeSlice(reader, 4);
    assert(maybeSlice.err == Read_Ok);
    logFn(Debug, "next chunk ID:\t\t%s\n", maybeSlice.slice);

    maybeSlice = fr_takeSlice(reader, 4);
    assert(maybeSlice.err == Read_Ok);
    uint32_t dataSize = bitcastU32_LE(maybeSlice.slice);
    logFn(Debug, "data size:\t\t%u\n", dataSize);

    uint32_t nBlocks = 8 * dataSize / bitDepth / nChannels;
    logFn(Debug, "n blocks:\t\t%u\n", nBlocks);
    float runtime = (float)nBlocks / (float)sampleRate;
    logFn(Debug, "runtime:\t\t%f secs\n", runtime);

    if (bitDepth != 8 && bitDepth != 16 && bitDepth != 24 && bitDepth != 32) {
        logFn(Error, "Unsupported bitdepth: %i\n", bitDepth);
        exit(1);
    }

    SampleFormat sampleFormat = 255;
    if (waveFormat == WAVE_FORMAT_IEEE_FLOAT) {
        sampleFormat = Float32;
    } else if (waveFormat == WAVE_FORMAT_PCM && bitDepth == 8) {
        sampleFormat = Unsigned8;
    } else if (waveFormat == WAVE_FORMAT_PCM && bitDepth == 16) {
        sampleFormat = Signed16;
    } else if (waveFormat == WAVE_FORMAT_PCM && bitDepth == 24) {
        sampleFormat = Signed24;
    } else if (waveFormat == WAVE_FORMAT_PCM && bitDepth == 32) {
        sampleFormat = Signed32;
    }

    if (sampleFormat == 255) {
        logFn(Error, "Failed to assign a sample format\n");
        exit(1);
    }

    logFn(Debug, "\n");
    return (Header){
        .nChannels = nChannels,
        .sampleRate = sampleRate,
        .sampleFormat = sampleFormat,
        .size = nBlocks,
        .runtimeMs = (uint32_t)(1000.0f * runtime),
    };
}

void logHeader(const Header* wh)
{
    logFn(Info, "%s {\n", "WavHeader");
    logFn(Info, "\tnumber of Channels:\t%u\n", wh->nChannels);
    logFn(Info, "\tsample rate:\t\t%u Hz\n", wh->sampleRate);
    logFn(Info, "\tsample format:\t\t%s\n", sampleFormatRepr(wh->sampleFormat));
    logFn(Info, "\tnumber of samples:\t%u\n", wh->size);
    logFn(Info, "}\n\n");
}
