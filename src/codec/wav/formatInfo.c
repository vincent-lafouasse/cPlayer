#include "wav_internals.h"

#include <assert.h>
#include <string.h>

#include "Error.h"
#include "bitcast.h"
#include "log.h"

Error skipChunkUntil(Reader* reader, const char* expectedId)
{
    uint8_t id[5] = {0};

    TRY(reader_peekFourCC(reader, id));
    while (memcmp(id, expectedId, 4) != 0) {
        // skip the fourCC we just peeked
        TRY(reader_skip(reader, 4));

        uint32_t chunkSize = 0;
        TRY(reader_takeU32_LE(reader, &chunkSize));
        logFn(LogLevel_Debug, "Skipping chunk %s of size %u\n", id, chunkSize);
        TRY(reader_skip(reader, chunkSize));
        TRY(reader_peekFourCC(reader, id));
    }
    return err_Ok();
}

static uint32_t fourCC_asU32(const uint8_t fourcc[4])
{
    return bitcastU32_BE(fourcc);
}

Error getToFormatChunk(Reader* reader)
{
    uint8_t id[5] = {0};
    TRY(reader_takeFourCC(reader, id));
    if (memcmp(id, "RIFF", 4) != 0) {
        return err_withCtx(E_Wav, EWav_UnknownFourCC, fourCC_asU32(id));
    }

    uint32_t size;
    TRY(reader_takeU32_LE(reader, &size));
    logFn(LogLevel_Debug, "master RIFF chunk:\t %u bytes\n", size);

    TRY(reader_takeFourCC(reader, id));
    if (memcmp(id, "WAVE", 4) != 0) {
        return err_withCtx(E_Wav, EWav_UnknownFourCC, fourCC_asU32(id));
    }

    return skipChunkUntil(reader, "fmt ");
}

Error readFormatChunk(Reader* reader, WavFormatChunk* out)
{
    Slice header;
    TRY(reader_takeSlice(reader, 8, &header));
    if (memcmp(header.slice, "fmt ", 4) != 0) {
        return err_withCtx(E_Wav, EWav_UnknownFourCC,
                           fourCC_asU32(header.slice));
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
    return err_Ok();
}

Error validateWavFormatChunk(const WavFormatChunk* format, SampleFormat* out)
{
    // Check number of channels (we only accept mono/stereo eventually)
    if (format->nChannels == 0 || format->nChannels > 2)
        return err_withCtx(E_Codec, ECdc_UnsupportedChannelLayout,
                           format->nChannels);

    // Check sample rate sanity (arbitrary max = 192 kHz)
    if (format->sampleRate == 0 || format->sampleRate > 192000)
        return err_withCtx(E_Codec, ECdc_AbsurdSampleRate, format->sampleRate);

    // Check bits per sample / block align consistency
    if (format->bitDepth == 0 || format->bitDepth > 64)
        return err_withCtx(E_Wav, EWav_InvalidBitDepth, format->bitDepth);

    // PCM sanity: blockAlign must match nChannels * bytesPerSample
    uint16_t bytesPerSample = (format->bitDepth + 7) / 8;
    if (format->formatTag == WAVE_FORMAT_PCM ||
        format->formatTag == WAVE_FORMAT_IEEE_FLOAT) {
        if (format->blockAlign != bytesPerSample * format->nChannels)
            return err_Err(E_Wav, EWav_BlockAlignMismatch);
    }

    // Extension size sanity
    if (format->size < 16)
        return err_withCtx(E_Wav, EWav_FormatChunkTooSmall, format->size);
    if (format->size == 18 && format->extensionSize != 0)
        return err_Err(E_Wav, EWav_ExtensionSizeMismatch);
    if (format->size == 40 && format->extensionSize != 22)
        return err_Err(E_Wav, EWav_ExtensionSizeMismatch);

    // Sample format mapping
    switch (format->formatTag) {
        case WAVE_FORMAT_PCM:
            switch (format->bitDepth) {
                case 8:
                    *out = SampleFormat_Unsigned8;
                    break;
                case 16:
                    *out = SampleFormat_Signed16;
                    break;
                case 24:
                    *out = SampleFormat_Signed24;
                    break;
                case 32:
                    *out = SampleFormat_Signed32;
                    break;
                default:
                    return err_withCtx(E_Wav, EWav_UnsupportedBitDepth,
                                       format->bitDepth);
            }
            break;
        case WAVE_FORMAT_IEEE_FLOAT:
            if (format->bitDepth == 32)
                *out = SampleFormat_Float32;
            else if (format->bitDepth == 64)
                *out = SampleFormat_Float64;
            else
                return err_withCtx(E_Wav, EWav_UnsupportedBitDepth,
                                   format->bitDepth);
            break;
        case WAVE_FORMAT_ADPCM:
            *out = SampleFormat_ADPCM;  // no validation yet
            break;
        case WAVE_FORMAT_MULAW:
            *out = SampleFormat_MULAW;  // no validation yet
            break;
        case WAVE_FORMAT_ALAW:
            *out = SampleFormat_ALAW;  // no validation yet
            break;
        default:
            return err_withCtx(E_Wav, EWav_UnknownSampleFormat,
                               format->formatTag);
    }

    return err_Ok();
}

Error readWavFormatInfo(Reader* reader, WavFormatInfo* out)
{
    // master chunk
    // skip other chunks
    TRY(getToFormatChunk(reader));

    WavFormatChunk format;
    TRY(readFormatChunk(reader, &format));
    TRY(skipChunkUntil(reader, "data"));

    SampleFormat sampleFormat;
    TRY(validateWavFormatChunk(&format, &sampleFormat));
    logFn(LogLevel_Debug, "format chunk seems good\n");

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

    *out = (WavFormatInfo){.formatTag = format.formatTag,
                           .nChannels = format.nChannels,
                           .sampleRate = format.sampleRate,
                           .sampleFormat = sampleFormat,
                           .blockAlign = format.blockAlign,
                           .nFrames = nBlocks,
                           .bitsPerSample = format.bitDepth,
                           .adpcmBlockSize = 0};
    logFn(LogLevel_Debug, "\n");
    return err_Ok();
}

static const char* formatTagRepr(uint16_t formatTag)
{
    switch (formatTag) {
        case WAVE_FORMAT_PCM:
            return "PCM";
        case WAVE_FORMAT_ADPCM:
            return "ADPCM";
        case WAVE_FORMAT_IEEE_FLOAT:
            return "IEEE Float";
        case WAVE_FORMAT_ALAW:
            return "A-Law";
        case WAVE_FORMAT_MULAW:
            return "mu-Law";
        case WAVE_FORMAT_EXTENSIBLE:
            return "Extended";
    }
    return "Unknown wav format";
}

void logWavFormatInfo(const WavFormatInfo* info)
{
    logFn(LogLevel_Info, "%s {\n", "WavFormatInfo");
    logFn(LogLevel_Info, "\tformat:\t\t\t%s\n", formatTagRepr(info->formatTag));
    logFn(LogLevel_Info, "\tnumber of Channels:\t%u\n", info->nChannels);
    logFn(LogLevel_Info, "\tsample rate:\t\t%u Hz\n", info->sampleRate);
    logFn(LogLevel_Info, "\tsample format:\t\t%s\n",
          sampleFormatRepr(info->sampleFormat));
    logFn(LogLevel_Info, "\tnumber of frames:\t%u\n", info->nFrames);
    logFn(LogLevel_Info, "\tblock align:\t\t%u\n", info->blockAlign);
    logFn(LogLevel_Info, "\tbit depth:\t\t%u\n", info->bitsPerSample);
    logFn(LogLevel_Info, "\tadpcm block size:\t%u\n", info->adpcmBlockSize);
    logFn(LogLevel_Info, "}\n\n");
}
