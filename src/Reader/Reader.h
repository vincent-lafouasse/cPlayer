#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "Error64.h"
#include "int24.h"

typedef struct Reader Reader;

typedef struct {
    const uint8_t* slice;
    size_t len;
} Slice;

typedef Error64 (*PeekSliceFn)(Reader* reader, size_t n, Slice* out);
typedef Error64 (*PeekIntoFn)(Reader* reader, size_t n, uint8_t* out);
typedef Error64 (*SkipFn)(Reader*, size_t);

struct Reader {
    void* ctx;
    PeekSliceFn peekSlice;
    PeekIntoFn peekInto;
    SkipFn skip;
    size_t offset;
};

Error64 reader_takeSlice(Reader* reader, size_t n, Slice* out);

Error64 reader_peekFourCC(Reader* reader, uint8_t* out);
Error64 reader_takeFourCC(Reader* reader, uint8_t* out);

Error64 reader_takeU16_LE(Reader* reader, uint16_t* out);
Error64 reader_takeI16_LE(Reader* reader, int16_t* out);
Error64 reader_takeU32_LE(Reader* reader, uint32_t* out);
Error64 reader_takeI32_LE(Reader* reader, int32_t* out);
Error64 reader_takeI24_LE(Reader* reader, Int24* out);
