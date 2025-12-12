#include "libcodec_wav.h"
#include "wav_internals.h"

#include <string.h>

#include "bitcast.h"

#include "log.h"

static uint32_t fourCC_asU32(const uint8_t fourcc[4])
{
    return bitcastU32_BE(fourcc);
}

WavError readFormatChunk(Reader* reader, WavFormatChunk* out)
{
    Slice header;
    TRY_IO(reader_takeSlice(reader, 8, &header));
    if (memcmp(header.slice, "fmt ", 4) != 0) {
        return EWav_UnknownFourCC;
    }
    logFn(LogLevel_Debug, "format chunk start:\t%u\n", reader->offset - 8);
    const uint32_t fmtChunkSize = bitcastU32_LE(header.slice + 4);
    logFn(LogLevel_Debug, "format chunk size:\t%u bytes\n", fmtChunkSize);

    Slice slice;
    TRY_IO(reader_takeSlice(reader, fmtChunkSize, &slice));
    const uint8_t* format = slice.slice;

    const uint16_t waveFormat = bitcastU16_LE(format);
    const uint16_t nChannels = bitcastU16_LE(format + 2);
    const uint32_t sampleRate = bitcastU32_LE(format + 4);
    const uint32_t bytesPerSec = bitcastU32_LE(format + 8);
    const uint16_t blockAlign = bitcastU16_LE(format + 12);
    const uint16_t bitDepth = bitcastU16_LE(format + 14);

    logFn(LogLevel_Debug, "wave format:\t\t0x%04:x\n", waveFormat);
    logFn(LogLevel_Debug, "n. channels:\t\t%x\n", nChannels);
    logFn(LogLevel_Debug, "sample rate:\t\t%u\n", sampleRate);
    logFn(LogLevel_Debug, "data rate:\t\t%u bytes per sec\n", bytesPerSec);
    logFn(LogLevel_Debug, "block align:\t\t%u bytes\n", blockAlign);
    logFn(LogLevel_Debug, "bit depth:\t\t%u bits\n", bitDepth);

    uint16_t formatChunkExtensionSize = fmtChunkSize - 16;
    logFn(LogLevel_Debug, "fmt extension:\t%u bytes\n",
          formatChunkExtensionSize);

    memset(out, 0, sizeof(*out));
    *out = (WavFormatChunk){
        .size = fmtChunkSize,
        .formatTag = waveFormat,
        .nChannels = nChannels,
        .sampleRate = sampleRate,
        .bytesPerSecond = bytesPerSec,
        .blockAlign = blockAlign,
        .bitDepth = bitDepth,
    };
    if (fmtChunkSize >= 18) {
        out->extensionSize = bitcastU16_LE(format + 16);
    }
    if (fmtChunkSize >= 20) {
        out->validBitsPerSample = bitcastU16_LE(format + 18);
    }
    if (fmtChunkSize >= 24) {
        out->channelMask = bitcastU32_LE(format + 20);
    }
    if (fmtChunkSize >= 40) {
        memcpy(out->subFormat, format + 24, 16);
    }
    return EWav_Ok;
}
