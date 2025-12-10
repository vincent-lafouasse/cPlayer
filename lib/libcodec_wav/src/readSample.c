#include "libcodec_wav.h"
#include "wav_internals.h"

#define INT24_MAX_ABS (1 << 23)
#define INT16_MAX_ABS (-INT16_MIN)

WavError readI24(Reader* reader, float* out)
{
    int32_t i24;
    TRY_IO(reader_takeI24_LE(reader, &i24));

    *out = (float)i24 / (float)INT24_MAX_ABS;
    return EWav_Ok;
}

WavError readI16(Reader* reader, float* out)
{
    int16_t i16;
    TRY_IO(reader_takeI16_LE(reader, &i16));

    *out = (float)i16 / (float)INT16_MAX_ABS;
    return EWav_Ok;
}

WavError readSample(Reader* reader, const WavFormatInfo* format, float* out)
{
    const size_t sampleBlockSize = format->blockAlign / format->nChannels;

    switch (format->sampleFormat) {
        case SampleFormat_Signed24:
            TRY(readI24(reader, out));
            return err_fromIo(reader_skip(reader, sampleBlockSize - 3));
        case SampleFormat_Signed16:
            TRY(readI16(reader, out));
            return err_fromIo(reader_skip(reader, sampleBlockSize - 2));
        default:
            return EWav_UnsupportedSampleFormat;
    }
}
