#pragma once

typedef enum {
    NoError = 0,
    E_OOM,

    E_Bad_Usage,

    E_Read_Error,
    E_Read_EOF,

    E_Codec_UnsupportedCodec,
    E_Codec_UnsupportedChannelLayout,

    E_Wav_UnknownFourCC,
    E_Wav_UnsupportedSampleFormat,
} Error;

static inline const char* errorRepr(Error e)
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
