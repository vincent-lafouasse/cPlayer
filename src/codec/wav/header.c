#include "Error.h"
#include "FileReader.h"
#include "bitcast.h"
#include "wav_internals.h"

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
    SliceResult maybeSlice = fr_takeSlice(reader, 12);
    if (maybeSlice.err != Read_Ok) {
        return translateError(maybeSlice.err);
    }

    // "RIFF" magic word, wav is a RIFF container
    const uint8_t* masterChunk = maybeSlice.slice;
    if (strncmp((const char*)masterChunk, "RIFF", 4) != 0) {
        return E_Wav_UnknownFourCC;
    }

    const uint32_t masterChunkSize = bitcastU32_LE(masterChunk + 4);
    logFn(LogLevel_Debug, "master chunk size:\t%u bytes\n", masterChunkSize);

    // the WAVE chunk contains our metadata and data
    if (strncmp((const char*)masterChunk + 8, "WAVE", 4) != 0) {
        return E_Wav_UnknownFourCC;
    }

    return NoError;
}

typedef struct {
    uint16_t formatTag;
    uint16_t nChannels;
    uint32_t sampleRate;
    uint16_t bitDepth;
    uint16_t blockSize;
} WavFormatChunk;

typedef struct {
    WavFormatChunk format;
    Error err;
} WavFormatChunkResult;

static inline WavFormatChunkResult WavFormatChunk_Ok(WavFormatChunk format)
{
    return (WavFormatChunkResult){.format = format, .err = NoError};
}

static inline WavFormatChunkResult WavFormatChunk_Err(Error err)
{
    return (WavFormatChunkResult){.err = err};
}

WavFormatChunkResult readFormatChunk(FileReader* reader)
{
    SliceResult maybeHeader = fr_takeSlice(reader, 8);
    if (maybeHeader.err != Read_Ok) {
        return WavFormatChunk_Err(translateError(maybeHeader.err));
    }
    const uint8_t* header = maybeHeader.slice;

    // head should be at the format chunk
    if (strncmp((const char*)header, "fmt ", 4) != 0) {
        return WavFormatChunk_Err(E_Wav_UnknownFourCC);
    }

    // size of the rest of the format chunk
    const uint32_t fmtChunkSize = bitcastU32_LE(header + 4);
    logFn(LogLevel_Debug, "format chunk size:\t%u bytes\n", fmtChunkSize);

    SliceResult maybeFormatChunk = fr_takeSlice(reader, fmtChunkSize);
    if (maybeFormatChunk.err != NoError) {
        return WavFormatChunk_Err(translateError(maybeFormatChunk.err));
    }
    const uint8_t* slice = maybeFormatChunk.slice;

    const uint16_t waveFormat = bitcastU16_LE(slice);
    const uint16_t nChannels = bitcastU16_LE(slice + 2);
    const uint32_t sampleRate = bitcastU32_LE(slice + 4);
    const uint32_t bytesPerSec = bitcastU32_LE(slice + 8);
    const uint16_t blockSize = bitcastU16_LE(slice + 12);
    const uint16_t bitDepth = bitcastU16_LE(slice + 14);

    logFn(LogLevel_Debug, "wave format:\t\t0x%04:x\n", waveFormat);
    logFn(LogLevel_Debug, "n. channels:\t\t%x\n", nChannels);
    logFn(LogLevel_Debug, "sample rate:\t\t%u\n", sampleRate);
    logFn(LogLevel_Debug, "data rate:\t\t%u bytes per sec\n", bytesPerSec);
    logFn(LogLevel_Debug, "block size:\t\t%u bytes\n", blockSize);
    logFn(LogLevel_Debug, "bit depth:\t\t%u bits\n", bitDepth);

    // not doing anything with the extension
    uint16_t formatChunkExtensionSize = fmtChunkSize - 16;
    logFn(LogLevel_Debug, "fmt extension:\t\t%u bytes\n",
          formatChunkExtensionSize);

    WavFormatChunk format = (WavFormatChunk){
        .formatTag = waveFormat,
        .nChannels = nChannels,
        .sampleRate = sampleRate,
        .bitDepth = bitDepth,
        .blockSize = blockSize,
    };
    return WavFormatChunk_Ok(format);
}

typedef struct {
    SampleFormat format;
    Error err;
} SampleFormatResult;

static inline SampleFormatResult sf_Ok(SampleFormat fmt)
{
    return (SampleFormatResult){.format = fmt, .err = NoError};
}

static inline SampleFormatResult sf_Err(Error err)
{
    return (SampleFormatResult){.err = err};
}

SampleFormatResult determineSampleFormat(WavFormatChunk format)
{
    if (format.formatTag != WAVE_FORMAT_PCM) {
        return sf_Err(E_Wav_UnsupportedSampleFormat);
    }

    const size_t sz = format.bitDepth / 8;
    if (sz == 1) {
        return sf_Ok(Unsigned8);
    } else if (sz == 2) {
        return sf_Ok(Signed16);
    } else if (sz == 3) {
        return sf_Ok(Signed32);
    } else if (sz == 4) {
        return sf_Ok(Signed32);
    } else {
        return sf_Err(E_Wav_UnsupportedSampleFormat);
    }
}

WavHeaderResult readWavHeader(FileReader* reader)
{
    // master chunk
    Error err = getToFormatChunk(reader);
    if (err != NoError) {
        return (WavHeaderResult){.err = err};
    }

    // format chunk
    WavFormatChunkResult maybeFormat = readFormatChunk(reader);
    if (maybeFormat.err != NoError) {
        return (WavHeaderResult){.err = maybeFormat.err};
    }
    WavFormatChunk format = maybeFormat.format;

    // data chunk
    SliceResult maybeSlice = fr_takeSlice(reader, 4);
    if (maybeSlice.err != Read_Ok) {
        return (WavHeaderResult){.err = translateError(maybeSlice.err)};
    }
    if (strncmp((const char*)maybeSlice.slice, "data", 4) != 0) {
        return (WavHeaderResult){.err = E_Wav_UnknownFourCC};
    }
    logFn(LogLevel_Debug, "next chunk ID:\t\t%s\n", maybeSlice.slice);

    maybeSlice = fr_takeSlice(reader, 4);
    if (maybeSlice.err != Read_Ok) {
        return (WavHeaderResult){.err = translateError(maybeSlice.err)};
    }
    uint32_t dataSize = bitcastU32_LE(maybeSlice.slice);
    logFn(LogLevel_Debug, "data size:\t\t%u\n", dataSize);

    uint32_t nBlocks = 8 * dataSize / format.bitDepth / format.nChannels;
    logFn(LogLevel_Debug, "n blocks:\t\t%u\n", nBlocks);
    logFn(LogLevel_Debug, "\n");

    SampleFormatResult maybeSampleFormat = determineSampleFormat(format);
    if (maybeSampleFormat.err != NoError) {
        return (WavHeaderResult){.err = maybeSampleFormat.err};
    }

    const Header h = (Header){
        .nChannels = format.nChannels,
        .sampleRate = format.sampleRate,
        .sampleFormat = maybeSampleFormat.format,
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
