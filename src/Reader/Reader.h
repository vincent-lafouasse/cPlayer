#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "Error.h"
#include "int24.h"

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

Error reader_peekFourCC(Reader* reader, uint8_t* out);
Error reader_takeFourCC(Reader* reader, uint8_t* out);

Error reader_takeU16(Reader* reader, uint16_t* out);
Error reader_takeU32(Reader* reader, uint32_t* out);
Error reader_takeI24(Reader* reader, Int24* out);
