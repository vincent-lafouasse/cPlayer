#pragma once

typedef enum {
    NoError = 0,
    E_OOM,

    E_Bad_Usage,
    E_Unknown_Flag,
    E_HelpRequested,

    E_Read_Error,
    E_Read_EOF,

    E_Codec_UnsupportedCodec,
    E_Codec_UnsupportedChannelLayout,

    E_Wav_UnknownFourCC,
    E_Wav_UnsupportedSampleFormat,

    E_Unimplemented,
} Error;

const char* errorRepr(Error e);
