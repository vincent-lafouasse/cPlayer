#pragma once

#include "Reader/Reader.h"
#include "audio.h"

typedef enum {
    Codec_Wav,
} Codec;

AudioDataResult decodeAudio(Reader* reader);
