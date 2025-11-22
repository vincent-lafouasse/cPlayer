#pragma once

#include "FileReader.h"

#include "Error.h"
#include "audio.h"

typedef enum {
    Codec_Wav,
} Codec;

AudioDataResult decodeAudio(FileReader* reader);
