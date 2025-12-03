#include "Error.h"

const char* errorRepr(Error e)
{
    switch (e) {
        case NoError:
            return "No error";
        case E_OOM:
            return "Out of memory";

        case E_Bad_Usage:
            return "Bad usage";

        case E_Read_Error:
            return "Read failed";
        case E_Read_EOF:
            return "Unexpected EOF when reading";

        case E_Codec_UnsupportedCodec:
            return "Unsupported codec";
        case E_Codec_UnsupportedChannelLayout:
            return "Unsupported channel layout";

        case E_Wav_UnknownFourCC:
            return "Unexpected RIFF 4CC";
        case E_Wav_UnsupportedSampleFormat:
            return "Unsupported sample format";
        default:
            return "Unkown error";
    }
}
