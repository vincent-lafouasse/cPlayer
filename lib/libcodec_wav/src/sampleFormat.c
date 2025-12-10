#include "wav_internals.h"

const char* sampleFormatRepr(SampleFormat fmt)
{
    switch (fmt) {
        case SampleFormat_Unsigned8:
            return "u8";
        case SampleFormat_Signed16:
            return "i16";
        case SampleFormat_Signed24:
            return "i24";
        case SampleFormat_Signed32:
            return "i32";
        case SampleFormat_Float32:
            return "f32";
        case SampleFormat_Float64:
            return "f64";
        case SampleFormat_ADPCM:
            return "ADPCM";
        case SampleFormat_MULAW:
            return "mu-law";
        case SampleFormat_ALAW:
            return "a-law";
    }
}
