#include "Error.h"
#include "wav_internals.h"

#include "int24.h"

#define INT24_MAX_ABS (1 << 23)

FloatResult readI24(FileReader* reader)
{
    const SliceResult maybeSlice = fr_takeSlice(reader, 3);
    if (maybeSlice.err == Read_Err) {
        return (FloatResult){.err = E_Read_Error};
    } else if (maybeSlice.err == Read_EOF) {
        return (FloatResult){.err = E_Read_EOF};
    }

    const Int24 i24 = bitcastI24_LE(maybeSlice.slice);
    const float f = (float)i24_asI32(i24) / (float)INT24_MAX_ABS;
    return (FloatResult){.f = f, .err = NoError};
}

FloatResult readSample(FileReader* reader, SampleFormat fmt)
{
    switch (fmt) {
        case Signed24:
            return readI24(reader);
        default:
            return (FloatResult){.err = E_Wav_UnsupportedSampleFormat};
    }
}
