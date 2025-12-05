#include "wav_internals.h"

#include "../codec_internals.h"
#include "Error.h"
#include "int24.h"

#define INT24_MAX_ABS (1 << 23)
#define INT16_MAX_ABS (-INT16_MIN)

Error readI24(Reader* reader, float* out)
{
    Int24 i24;
    TRY(reader_takeI24_LE(reader, &i24));

    *out = (float)i24_asI32(i24) / (float)INT24_MAX_ABS;
    return NoError;
}

Error readI16(Reader* reader, float* out)
{
    int16_t i16;
    TRY(reader_takeI16_LE(reader, &i16));

    *out = (float)i16 / (float)INT16_MAX_ABS;
    return NoError;
}

Error readSample(Reader* reader, const WavFormatInfo* format, float* out)
{
    const size_t sampleBlockSize = format->blockAlign / format->nChannels;

    switch (format->sampleFormat) {
        case SampleFormat_Signed24:
            TRY(readI24(reader, out));
            return reader->skip(reader, sampleBlockSize - 3);
        case SampleFormat_Signed16:
            TRY(readI16(reader, out));
            return reader->skip(reader, sampleBlockSize - 2);
        default:
            return E_Wav_UnsupportedSampleFormat;
    }
}
