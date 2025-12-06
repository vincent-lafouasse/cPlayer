#pragma once

#ifdef __cplusplus
extern "C" {
#endif

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
typedef Error (*SkipFn)(Reader* reader, size_t n);

struct Reader {
    void* ctx;
    PeekSliceFn peekSlice;
    PeekIntoFn peekInto;
    SkipFn skip;
    size_t offset;
};

Error reader_peekSlice(Reader* reader, size_t n, Slice* out);
Error reader_peekInto(Reader* reader, size_t n, uint8_t* out);
Error reader_skip(Reader* reader, size_t n);

Error reader_takeSlice(Reader* reader, size_t n, Slice* out);

Error reader_peekFourCC(Reader* reader, uint8_t* out);
Error reader_takeFourCC(Reader* reader, uint8_t* out);

Error reader_takeU16_LE(Reader* reader, uint16_t* out);
Error reader_takeI16_LE(Reader* reader, int16_t* out);
Error reader_takeU32_LE(Reader* reader, uint32_t* out);
Error reader_takeI32_LE(Reader* reader, int32_t* out);
Error reader_takeI24_LE(Reader* reader, Int24* out);

#ifdef __cplusplus
}
#endif
