#include "wav_internals.h"

#include <stdlib.h>

#include "int24.h"
#include "log.h"

#define INT24_MAX_ABS (1 << 23)

ReadResult readI24(FileReader* reader, float* out)
{
    uint8_t bytes[3];

    const ReadResult res = fr_takeSlice(reader, bytes, 3);
    if (res != Read_Ok) {
        return res;
    }

    Int24 e = deserializeI24_LE(bytes);
    *out = (float)i24_asI32(e) / (float)INT24_MAX_ABS;
    return Read_Ok;
}

ReadResult readSample(FileReader* reader, SampleFormat fmt, float* out)
{
    switch (fmt) {
        case Signed24:
            return readI24(reader, out);
        default:
            logFn(Error, "Unsupported sample format %i\n", fmt);
            exit(1);
    }
}
