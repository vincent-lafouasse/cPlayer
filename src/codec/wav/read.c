#include "wav_internals.h"

#include "Error.h"
#include "int24.h"

#define INT24_MAX_ABS (1 << 23)

FloatResult readI24(FileReader* reader)
{
    const SliceResult maybeSlice = fr_takeSlice(reader, 3);
    if (maybeSlice.status != ReadStatus_Ok) {
        return FloatResult_Err(error_fromReadStatus(maybeSlice.status));
    }

    const Int24 i24 = bitcastI24_LE(maybeSlice.slice);
    const float f = (float)i24_asI32(i24) / (float)INT24_MAX_ABS;
    return FloatResult_Ok(f);
}

FloatResult readSample(FileReader* reader, SampleFormat fmt)
{
    switch (fmt) {
        case Signed24:
            return readI24(reader);
        default:
            return FloatResult_Err(E_Wav_UnsupportedSampleFormat);
    }
}
