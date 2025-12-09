#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "audio.h"
#include "libstream/Reader.h"

typedef enum {
    Codec_Wav,
} Codec;

Error decodeAudio(Reader* reader, AudioData* out);

#ifdef __cplusplus
}
#endif
