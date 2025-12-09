#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Reader.h"
#include "audio.h"

AudioDataResult decodeWav(Reader* reader);

#ifdef __cplusplus
}
#endif
