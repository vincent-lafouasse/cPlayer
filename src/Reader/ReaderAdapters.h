#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "libstream/Reader.h"

#include "FileReader.h"

Reader reader_fromFileReader(FileReader* fileReader);

#ifdef __cplusplus
}
#endif
