#pragma once

typedef enum {
    NoError = 0,
    E_OOM,

    E_Read_Error,
    E_Read_EOF,

    E_Codec_UnsupportedCodec,
    E_Wav_UnknownFourCC,
} Error;
