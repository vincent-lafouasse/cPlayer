#pragma once

#include "audio.h"
#include "Reader/Reader.h"

typedef enum {
    Codec_Wav,
} Codec;

AudioDataResult decodeAudio(Reader* reader);
