#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "libstream/Reader.h"

#include "Error.h"
#include "audio.h"

Error decodeWav(Reader* reader, AudioData* out);

#ifdef __cplusplus
}
#endif
