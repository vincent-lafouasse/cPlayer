#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "libaudiospecs.h"
#include "libstream/Reader.h"

#include "Error.h"

Error decodeWav(Reader* reader, AudioBuffer* out);

#ifdef __cplusplus
}
#endif
