#include "wav_internals.h"

#include <stdlib.h>

#include "int24.h"
#include "log.h"

#define INT24_MAX_ABS (1 << 23)

FloatResult readI24(FileReader* reader)
{
    const SliceResult maybeSlice = fr_takeSlice(reader, 3);
    if (maybeSlice.err != Read_Ok) {
        return (FloatResult){.err = maybeSlice.err};
    }

    const Int24 i24 = bitcastI24_LE(maybeSlice.slice);
    const float f = (float)i24_asI32(i24) / (float)INT24_MAX_ABS;
    return (FloatResult){.f = f, .err = Read_Ok};
}

FloatResult readSample(FileReader* reader, SampleFormat fmt)
{
    switch (fmt) {
        case Signed24:
            return readI24(reader);
        default:
            logFn(LogLevel_Error, "Unsupported sample format %i\n", fmt);
            exit(1);
    }
}
