#pragma once

#include "FileReader.h"

#include "audio.h"

typedef enum {
    Codec_Wav,
} Codec;

typedef enum {
    DecodingError_None,
} DecodingError;

typedef struct {
    AudioData track;
    DecodingError err;
} DecodingResult;

DecodingResult decodeAudio(FileReader* reader);
