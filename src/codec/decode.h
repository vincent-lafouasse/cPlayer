#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "libstream/Reader.h"
#include "audio.h"

typedef enum {
    Codec_Wav,
} Codec;

AudioDataResult decodeAudio(Reader* reader);

#ifdef __cplusplus
}
#endif
