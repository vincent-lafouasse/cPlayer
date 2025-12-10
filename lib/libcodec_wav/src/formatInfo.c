#include "wav_internals.h"

#include <assert.h>
#include <string.h>

#include "bitcast.h"

#include "log.h"

WavError readWavFormatInfo(Reader* reader, WavFormatInfo* out)
{
    // master chunk
    // skip other chunks
    TRY(getToFormatChunk(reader));

    WavFormatChunk formatChunk;
    TRY(readFormatChunk(reader, &formatChunk));

    WavFormatInfo format;
    TRY(parseFormatChunk(&formatChunk, &format));

    // only keep int and float PCM
    // no ADPCM, no A-Law, no mu-Law
    TRY(checkFormatSupport(&format));
    // 0xfffe should have been collapsed into PCM by this point
    assert(format.formatTag == WAVE_FORMAT_PCM ||
           format.formatTag == WAVE_FORMAT_IEEE_FLOAT);

    TRY(skipChunkUntil(reader, "data"));
    // data chunk
    Slice dataChunkHeader;
    TRY_IO(reader_takeSlice(reader, 8, &dataChunkHeader));
    assert(memcmp(dataChunkHeader.slice, "data", 4) == 0);
    logFn(LogLevel_Debug, "reached data chunk\n");

    uint32_t dataSize = bitcastU32_LE(dataChunkHeader.slice + 4);
    logFn(LogLevel_Debug, "data size:\t\t%u\n", dataSize);

    // NOTE: this won't be true for ADPCM
    format.nFrames = dataSize / format.blockAlign;
    logFn(LogLevel_Debug, "n frames:\t\t%u\n", format.nFrames);

    *out = format;
    logFn(LogLevel_Debug, "\n");
    return EWav_Ok;
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
    logFn(LogLevel_Info, "\tbit depth:\t\t%u\n", info->bitDepth);
    logFn(LogLevel_Info, "\tadpcm block size:\t%u\n", info->adpcmBlockSize);
    logFn(LogLevel_Info, "}\n\n");
}
