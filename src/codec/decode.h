#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "audio.h"
#include "libstream/Reader.h"

typedef enum {
    Codec_Wav,
} Codec;

AudioDataResult decodeAudio(Reader* reader);

#ifdef __cplusplus
}
#endif
