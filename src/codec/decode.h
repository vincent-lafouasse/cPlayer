#pragma once

#include "FileReader.h"

#include "Error.h"
#include "audio.h"

typedef enum {
    Codec_Wav,
} Codec;

typedef struct {
    AudioData track;
    Error err;
} DecodingResult;

DecodingResult decodeAudio(FileReader* reader);
