#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "libstream/Reader.h"

#include "Error.h"
#include "audio.h"

typedef enum {
    Codec_Wav,
} Codec;

Error decodeAudio(Reader* reader, AudioData* out);

#ifdef __cplusplus
}
#endif
