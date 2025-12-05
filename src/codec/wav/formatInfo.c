#include "wav_internals.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../codec_internals.h"
#include "Error.h"
#include "bitcast.h"

#include "log.h"

Error skipChunkUntil(Reader* reader, const char* expectedId)
{
    uint8_t id[5] = {0};

    TRY(reader_peekFourCC(reader, id));
    while (memcmp(id, expectedId, 4) != 0) {
        // skip the fourCC we just peeked
        TRY(reader->skip(reader, 4));

        uint32_t chunkSize = 0;
        TRY(reader_takeU32_LE(reader, &chunkSize));
        logFn(LogLevel_Debug, "Skipping chunk %s of size %u\n", id, chunkSize);
        TRY(reader->skip(reader, chunkSize));
        TRY(reader_peekFourCC(reader, id));
    }
    return NoError;
}

Error getToFormatChunk(Reader* reader)
{
    uint8_t id[5] = {0};
    TRY(reader_takeFourCC(reader, id));
    if (memcmp(id, "RIFF", 4) != 0) {
        return E_Wav_UnknownFourCC;
    }

    uint32_t size;
    TRY(reader_takeU32_LE(reader, &size));
    logFn(LogLevel_Debug, "master RIFF chunk:\t %u bytes\n", size);

    TRY(reader_takeFourCC(reader, id));
    if (memcmp(id, "WAVE", 4) != 0) {
        return E_Wav_UnknownFourCC;
    }

    return skipChunkUntil(reader, "fmt ");
}

Error readFormatChunk(Reader* reader, WavFormatChunk* out)
{
    Slice header;
    TRY(reader_takeSlice(reader, 8, &header));
    if (memcmp(header.slice, "fmt ", 4) != 0) {
        return E_Wav_UnknownFourCC;
    }
    const uint32_t fmtChunkSize = bitcastU32_LE(header.slice + 4);
    logFn(LogLevel_Debug, "format chunk size:\t%u bytes\n", fmtChunkSize);

    Slice slice;
    TRY(reader_takeSlice(reader, fmtChunkSize, &slice));
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
    logFn(LogLevel_Debug, "fmt extension:\t\t%u bytes\n",
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
    return NoError;
}

Error determineSampleFormat(WavFormatChunk format, SampleFormat* out)
{
    if (format.formatTag != WAVE_FORMAT_PCM) {
        return E_Wav_UnsupportedSampleFormat;
    }

    Error err = NoError;
    const size_t sz = format.bitDepth / 8;
    if (sz == 1) {
        *out = SampleFormat_Unsigned8;
    } else if (sz == 2) {
        *out = SampleFormat_Signed16;
    } else if (sz == 3) {
        *out = SampleFormat_Signed24;
    } else if (sz == 4) {
        *out = SampleFormat_Signed32;
    } else {
        err = E_Wav_UnsupportedSampleFormat;
    }

    return err;
}

Error readWavFormatInfo(Reader* reader, WavFormatInfo* out)
{
    // master chunk
    // skip other chunks
    TRY(getToFormatChunk(reader));

    WavFormatChunk format;
    TRY(readFormatChunk(reader, &format));
    TRY(skipChunkUntil(reader, "data"));

    // data chunk
    Slice dataChunkHeader;
    TRY(reader_takeSlice(reader, 8, &dataChunkHeader));
    assert(memcmp(dataChunkHeader.slice, "data", 4) == 0);
    logFn(LogLevel_Debug, "reached data chunk\n");

    uint32_t dataSize = bitcastU32_LE(dataChunkHeader.slice + 4);
    logFn(LogLevel_Debug, "data size:\t\t%u\n", dataSize);

    // NOTE: this won't be true for ADPCM
    uint32_t nBlocks = 8 * dataSize / format.bitDepth / format.nChannels;
    logFn(LogLevel_Debug, "n blocks:\t\t%u\n", nBlocks);

    SampleFormat sampleFormat;
    TRY(determineSampleFormat(format, &sampleFormat));

    *out = (WavFormatInfo){.nChannels = format.nChannels,
                           .sampleRate = format.sampleRate,
                           .sampleFormat = sampleFormat,
                           .blockAlign = format.blockAlign,
                           .nFrames = nBlocks,
                           .bitsPerSample = format.bitDepth,
                           .adpcmBlockSize = 0};
    logFn(LogLevel_Debug, "\n");
    return NoError;
}

void logWavFormatInfo(const WavFormatInfo* info)
{
    logFn(LogLevel_Info, "%s {\n", "WavFormatInfo");
    logFn(LogLevel_Info, "\tnumber of Channels:\t%u\n", info->nChannels);
    logFn(LogLevel_Info, "\tsample rate:\t\t%u Hz\n", info->sampleRate);
    logFn(LogLevel_Info, "\tsample format:\t\t%s\n",
          sampleFormatRepr(info->sampleFormat));
    logFn(LogLevel_Info, "\tnumber of frames:\t%u\n", info->nFrames);
    logFn(LogLevel_Info, "\tblock align:\t%u\n", info->blockAlign);
    logFn(LogLevel_Info, "\tbit depth:\t\t%u\n", info->bitsPerSample);
    logFn(LogLevel_Info, "\tadpcm block size:\t%u\n", info->adpcmBlockSize);
    logFn(LogLevel_Info, "}\n\n");
}
