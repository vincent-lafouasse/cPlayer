#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "libaudiospecs.h"
#include "libstream/Reader.h"

#include "Error.h"

typedef enum {
    Codec_Wav,
} Codec;

Error decodeAudio(Reader* reader, AudioBuffer* out);

#ifdef __cplusplus
}
#endif
