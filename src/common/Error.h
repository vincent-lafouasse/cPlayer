#pragma once

#include "FileReader.h"

typedef enum {
    NoError = 0,
    E_OOM,

    E_Bad_Usage,
    E_Unknown_Flag,
    E_HelpRequested,

    E_FailedRead,
    E_UnexpectedEOF,

    E_Codec_UnsupportedCodec,
    E_Codec_UnsupportedChannelLayout,

    E_Wav_UnknownFourCC,
    E_Wav_UnsupportedSampleFormat,

    E_Unimplemented,
} Error;

const char* errorRepr(Error e);
Error error_fromReadStatus(ReadStatus readStatus);
