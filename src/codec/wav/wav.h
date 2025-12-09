#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "audio.h"
#include "libstream/Reader.h"

AudioDataResult decodeWav(Reader* reader);

#ifdef __cplusplus
}
#endif
