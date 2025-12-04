#include "wav_internals.h"

#include <stdlib.h>
#include <string.h>

#include "../codec_internals.h"
#include "Error.h"
#include "FileReader.h"
#include "bitcast.h"

#include "log.h"

static Error readFourCC(FileReader* reader, const char* expected)
{
    SliceResult fourCC = fr_takeSlice(reader, 4);
    if (fourCC.status != ReadStatus_Ok) {
        return error_fromReadStatus(fourCC.status);
    }

    if (memcmp(fourCC.slice, expected, 4) != 0) {
        return E_Wav_UnknownFourCC;
    } else {
        return NoError;
    }
}

static Error readU16(FileReader* reader, uint16_t* out)
{
    SliceResult slice = fr_takeSlice(reader, 2);
    if (slice.status != ReadStatus_Ok) {
        return error_fromReadStatus(slice.status);
    }

    return bitcastU16_LE(slice.slice);
}

static Error readU32(FileReader* reader, uint32_t* out)
{
    SliceResult slice = fr_takeSlice(reader, 4);
    if (slice.status != ReadStatus_Ok) {
        return error_fromReadStatus(slice.status);
    }

    return bitcastU32_LE(slice.slice);
}

static Error skip(FileReader* reader, size_t n)
{
    return error_fromReadStatus(fr_skip(reader, n));
}

static Error skipChunkUntil(FileReader* reader, const char* expectedId)
{
    SliceResult fourCC = fr_peekSlice(reader, 4);
    if (fourCC.status != ReadStatus_Ok) {
        return error_fromReadStatus(fourCC.status);
    }
    while (memcmp(fourCC.slice, expectedId, 4) != 0) {
        Error err = NoError;

        char id[5] = {0};
        memcpy(id, fourCC.slice, 4);

        uint32_t chunkSize = 0;
        if ((err = readU32(reader, &chunkSize)) != ReadStatus_Ok) {
            return err;
        }
        logFn(LogLevel_Debug, "Skipping chunk %s of size %u\n", id, chunkSize);
        if ((err = skip(reader, chunkSize)) != ReadStatus_Ok) {
            return err;
        }
    }
    return NoError;
}

static Error getToFormatChunk(FileReader* reader)
{
    SliceResult maybeSlice = fr_takeSlice(reader, 12);
    if (maybeSlice.status != ReadStatus_Ok) {
        return error_fromReadStatus(maybeSlice.status);
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
static inline WavFormatChunkResult WavFormatChunk_ReadErr(ReadStatus readStatus)
{
    return (WavFormatChunkResult){.err = error_fromReadStatus(readStatus)};
}

WavFormatChunkResult readFormatChunk(FileReader* reader)
{
    SliceResult maybeHeader = fr_takeSlice(reader, 8);
    if (maybeHeader.status != ReadStatus_Ok) {
        return WavFormatChunk_ReadErr(maybeHeader.status);
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
    if (maybeFormatChunk.status != ReadStatus_Ok) {
        return WavFormatChunk_ReadErr(maybeFormatChunk.status);
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

static inline WavHeaderResult WavHeader_Ok(Header header)
{
    return (WavHeaderResult){.header = header, .err = NoError};
}

static inline WavHeaderResult WavHeader_Err(Error err)
{
    return (WavHeaderResult){.err = err};
}
static inline WavHeaderResult WavHeader_ReadErr(ReadStatus readStatus)
{
    return (WavHeaderResult){.err = error_fromReadStatus(readStatus)};
}

WavHeaderResult readWavHeader(FileReader* reader)
{
    // master chunk
    Error err = getToFormatChunk(reader);
    if (err != NoError) {
        return WavHeader_Err(err);
    }

    // format chunk
    WavFormatChunkResult maybeFormat = readFormatChunk(reader);
    if (maybeFormat.err != NoError) {
        return WavHeader_Err(maybeFormat.err);
    }
    WavFormatChunk format = maybeFormat.format;

    // data chunk
    SliceResult maybeSlice = fr_takeSlice(reader, 4);
    if (maybeSlice.status != ReadStatus_Ok) {
        return WavHeader_ReadErr(maybeSlice.status);
    }
    if (strncmp((const char*)maybeSlice.slice, "data", 4) != 0) {
        return WavHeader_Err(E_Wav_UnknownFourCC);
    }
    logFn(LogLevel_Debug, "next chunk ID:\t\t%s\n", maybeSlice.slice);

    maybeSlice = fr_takeSlice(reader, 4);
    if (maybeSlice.status != ReadStatus_Ok) {
        return WavHeader_ReadErr(maybeSlice.status);
    }
    uint32_t dataSize = bitcastU32_LE(maybeSlice.slice);
    logFn(LogLevel_Debug, "data size:\t\t%u\n", dataSize);

    uint32_t nBlocks = 8 * dataSize / format.bitDepth / format.nChannels;
    logFn(LogLevel_Debug, "n blocks:\t\t%u\n", nBlocks);
    logFn(LogLevel_Debug, "\n");

    SampleFormatResult maybeSampleFormat = determineSampleFormat(format);
    if (maybeSampleFormat.err != NoError) {
        return WavHeader_Err(maybeSampleFormat.err);
    }

    const Header h = (Header){
        .nChannels = format.nChannels,
        .sampleRate = format.sampleRate,
        .sampleFormat = maybeSampleFormat.format,
        .size = nBlocks,
    };

    return WavHeader_Ok(h);
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
