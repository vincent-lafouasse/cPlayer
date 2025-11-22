#include "wav_internals.h"

const char* sampleFormatRepr(SampleFormat fmt)
{
    switch (fmt) {
        case Unsigned8:
            return "u8";
        case Signed16:
            return "i16";
        case Signed24:
            return "i24";
        case Signed32:
            return "i32";
        case Float32:
            return "f32";
    }
}
