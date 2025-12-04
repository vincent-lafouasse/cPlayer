#include "wav_internals.h"

#include "../codec_internals.h"
#include "Error.h"
#include "int24.h"

#define INT24_MAX_ABS (1 << 23)

Error readI24(Reader* reader, float* out)
{
    Int24 i24;
    Error err = reader_takeI24_LE(reader, &i24);
    if (err != NoError) {
        return err;
    }

    *out = (float)i24_asI32(i24) / (float)INT24_MAX_ABS;
    return NoError;
}

Error readSample(Reader* reader, SampleFormat fmt, float* out)
{
    switch (fmt) {
        case Signed24:
            return readI24(reader, out);
        default:
            return E_Wav_UnsupportedSampleFormat;
    }
}
