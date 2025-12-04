#pragma once

#include <stdbool.h>

#include "FileReader.h"
#include "Error.h"

typedef struct Reader Reader;

typedef struct {
    const uint8_t* slice;
    size_t len;
} Slice;

typedef Error (*PeekSliceFn)(Reader* reader, size_t n, Slice* out);
typedef Error (*PeekIntoFn)(Reader* reader, size_t n, uint8_t* out);
typedef Error (*SkipFn)(Reader*, size_t);

struct Reader {
    void* ctx;
    PeekSliceFn peekSlice;
    PeekIntoFn peekInto;
    SkipFn skip;
    size_t offset;
};

Reader reader_fromFileReader(FileReader* fileReader);
