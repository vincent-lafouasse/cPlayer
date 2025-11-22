#include "Error.h"
#include "FileReader.h"
#include "bitcast.h"
#include "wav_internals.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

static Error translateError(ReadError re)
{
    switch (re) {
        case Read_Err:
            return E_Read_Error;
        case Read_EOF:
            return E_Read_EOF;
        default:
            return NoError;
    }
}

static Error getToFormatChunk(FileReader* reader)
{
    SliceResult maybeSlice = fr_takeSlice(reader, 4);
    if (maybeSlice.err != Read_Ok) {
        return translateError(maybeSlice.err);
    }
    if (strncmp((const char*)maybeSlice.slice, "RIFF", 4) != 0) {
        return E_Wav_UnknownFourCC;
    }
    logFn(LogLevel_Debug, "master chunk ID:\t%s\n", maybeSlice.slice);

    maybeSlice = fr_takeSlice(reader, 4);
    if (maybeSlice.err != Read_Ok) {
        return translateError(maybeSlice.err);
    }
    const uint32_t masterChunkSize = bitcastU32_LE(maybeSlice.slice);
    logFn(LogLevel_Debug, "master chunk size:\t%u bytes\n", masterChunkSize);

    maybeSlice = fr_takeSlice(reader, 4);
    if (maybeSlice.err != Read_Ok) {
        return translateError(maybeSlice.err);
    }
    if (strncmp((const char*)maybeSlice.slice, "WAVE", 4) != 0) {
        return E_Wav_UnknownFourCC;
    }
    logFn(LogLevel_Debug, "wav chunk ID:\t\t%s\n", maybeSlice.slice);

    return NoError;
}

WavHeaderResult readWavHeader(FileReader* reader)
{
    Error err = getToFormatChunk(reader);
    if (err != NoError) {
        return (WavHeaderResult){.err = err};
    }

    SliceResult maybeSlice = fr_takeSlice(reader, 4);
    assert(maybeSlice.err == Read_Ok);
    assert(strncmp((const char*)maybeSlice.slice, "fmt ", 4) == 0);
    logFn(LogLevel_Debug, "fmt chunk ID:\t\t%s\n", maybeSlice.slice);

    maybeSlice = fr_takeSlice(reader, 4);
    assert(maybeSlice.err == Read_Ok);
    const uint32_t fmtChunkSize = bitcastU32_LE(maybeSlice.slice);
    logFn(LogLevel_Debug, "format chunk size:\t%u bytes\n", fmtChunkSize);

    maybeSlice = fr_takeSlice(reader, 2);
    assert(maybeSlice.err == Read_Ok);
    const uint16_t waveFormat = bitcastU16_LE(maybeSlice.slice);
    logFn(LogLevel_Debug, "wave format:\t\t0x%04:x\n", waveFormat);
    if (waveFormat != WAVE_FORMAT_PCM) {
        logFn(LogLevel_Error, "\nError:\n\tUnsupported wave format\n");
        logFn(LogLevel_Error, "Only PCM is supported for now\n");
        exit(1);
    }

    maybeSlice = fr_takeSlice(reader, 2);
    assert(maybeSlice.err == Read_Ok);
    const uint32_t nChannels = bitcastU16_LE(maybeSlice.slice);
    logFn(LogLevel_Debug, "n. channels:\t\t%x\n", nChannels);

    maybeSlice = fr_takeSlice(reader, 4);
    assert(maybeSlice.err == Read_Ok);
    const uint32_t sampleRate = bitcastU32_LE(maybeSlice.slice);
    logFn(LogLevel_Debug, "sample rate:\t\t%u\n", sampleRate);

    maybeSlice = fr_takeSlice(reader, 4);
    assert(maybeSlice.err == Read_Ok);
    const uint32_t bytesPerSec = bitcastU32_LE(maybeSlice.slice);
    logFn(LogLevel_Debug, "data rate:\t\t%u bytes per sec\n", bytesPerSec);

    maybeSlice = fr_takeSlice(reader, 2);
    assert(maybeSlice.err == Read_Ok);
    const uint16_t blockSize = bitcastU16_LE(maybeSlice.slice);
    logFn(LogLevel_Debug, "block size:\t\t%u bytes\n", blockSize);

    maybeSlice = fr_takeSlice(reader, 2);
    assert(maybeSlice.err == Read_Ok);
    const uint16_t bitDepth = bitcastU16_LE(maybeSlice.slice);
    logFn(LogLevel_Debug, "bit depth:\t\t%u bits\n", bitDepth);

    uint16_t formatChunkExtensionSize = fmtChunkSize - 16;
    logFn(LogLevel_Debug, "fmt extension:\t\t%u bytes\n",
          formatChunkExtensionSize);
    assert(fr_skip(reader, formatChunkExtensionSize) == Read_Ok);

    maybeSlice = fr_takeSlice(reader, 4);
    assert(maybeSlice.err == Read_Ok);
    logFn(LogLevel_Debug, "next chunk ID:\t\t%s\n", maybeSlice.slice);

    maybeSlice = fr_takeSlice(reader, 4);
    assert(maybeSlice.err == Read_Ok);
    uint32_t dataSize = bitcastU32_LE(maybeSlice.slice);
    logFn(LogLevel_Debug, "data size:\t\t%u\n", dataSize);

    uint32_t nBlocks = 8 * dataSize / bitDepth / nChannels;
    logFn(LogLevel_Debug, "n blocks:\t\t%u\n", nBlocks);
    float runtime = (float)nBlocks / (float)sampleRate;
    logFn(LogLevel_Debug, "runtime:\t\t%f secs\n", runtime);

    if (bitDepth != 8 && bitDepth != 16 && bitDepth != 24 && bitDepth != 32) {
        logFn(LogLevel_Error, "Unsupported bitdepth: %i\n", bitDepth);
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
        logFn(LogLevel_Error, "Failed to assign a sample format\n");
        exit(1);
    }

    logFn(LogLevel_Debug, "\n");
    const Header h = (Header){
        .nChannels = nChannels,
        .sampleRate = sampleRate,
        .sampleFormat = sampleFormat,
        .size = nBlocks,
    };

    return (WavHeaderResult){.header = h, .err = NoError};
}

void logHeader(const Header* wh)
{
    logFn(LogLevel_Info, "%s {\n", "WavHeader");
    logFn(LogLevel_Info, "\tnumber of Channels:\t%u\n", wh->nChannels);
    logFn(LogLevel_Info, "\tsample rate:\t\t%u Hz\n", wh->sampleRate);
    logFn(LogLevel_Info, "\tsample format:\t\t%s\n",
          sampleFormatRepr(wh->sampleFormat));
    logFn(LogLevel_Info, "\tnumber of samples:\t%u\n", wh->size);
    logFn(LogLevel_Info, "}\n\n");
}
