#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "audio.h"
#include "libstream/Reader.h"

Error decodeWav(Reader* reader, AudioData* out);

#ifdef __cplusplus
}
#endif
