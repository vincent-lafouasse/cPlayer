#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
    ReadStatus_Ok,
    ReadStatus_ReadFailed,
    ReadStatus_UnexpectedEOF,
} ReadStatus;

typedef struct Reader Reader;

typedef struct {
    const uint8_t* slice;
    size_t len;
} Slice;

typedef ReadStatus (*PeekSliceFn)(Reader* reader, size_t n, Slice* out);
typedef ReadStatus (*SkipFn)(Reader* reader, size_t n);

struct Reader {
    void* ctx;
    PeekSliceFn peekSlice;
    SkipFn skip;
    size_t offset;
};

ReadStatus reader_peekSlice(Reader* reader, size_t n, Slice* out);
ReadStatus reader_peekInto(Reader* reader, size_t n, uint8_t* out);
ReadStatus reader_skip(Reader* reader, size_t n);

ReadStatus reader_takeSlice(Reader* reader, size_t n, Slice* out);

ReadStatus reader_peekFourCC(Reader* reader, uint8_t* out);
ReadStatus reader_takeFourCC(Reader* reader, uint8_t* out);

ReadStatus reader_takeU16_LE(Reader* reader, uint16_t* out);
ReadStatus reader_takeI16_LE(Reader* reader, int16_t* out);
ReadStatus reader_takeU32_LE(Reader* reader, uint32_t* out);
ReadStatus reader_takeI32_LE(Reader* reader, int32_t* out);
ReadStatus reader_takeI24_LE(Reader* reader, int32_t* out);

#ifdef __cplusplus
}
#endif
