#include "Reader.h"

#include <string.h>

#include "bitcast.h"
#include "int24.h"

Error reader_peekSlice(Reader* reader, size_t n, Slice* out)
{
    return reader->peekSlice(reader, n, out);
}

Error reader_peekInto(Reader* reader, size_t n, uint8_t* out)
{
    Slice slice;
    Error err = reader_peekSlice(reader, n, &slice);
    if (err_isOk(err)) {
        memcpy(out, slice.slice, n);
    }
    return err;
}

Error reader_skip(Reader* reader, size_t n)
{
    return reader->skip(reader, n);
}

Error reader_takeSlice(Reader* reader, size_t n, Slice* out)
{
    Error err = reader_peekSlice(reader, n, out);

    if (err_isOk(err)) {
        TRY(reader_skip(reader, n));
    }
    return err;
}

Error reader_peekFourCC(Reader* reader, uint8_t* out)
{
    Slice slice;
    TRY(reader_peekSlice(reader, 4, &slice));

    memcpy(out, slice.slice, 4);
    return err_Ok();
}

Error reader_takeFourCC(Reader* reader, uint8_t* out)
{
    TRY(reader_peekFourCC(reader, out));

    return reader_skip(reader, 4);
}

Error reader_takeU16_LE(Reader* reader, uint16_t* out)
{
    Slice slice;
    TRY(reader_peekSlice(reader, 2, &slice));

    *out = bitcastU16_LE(slice.slice);
    return reader_skip(reader, 2);
}

Error reader_takeI16_LE(Reader* reader, int16_t* out)
{
    Slice slice;
    TRY(reader_peekSlice(reader, 2, &slice));

    *out = bitcastI16_LE(slice.slice);
    return reader_skip(reader, 2);
}

Error reader_takeU32_LE(Reader* reader, uint32_t* out)
{
    Slice slice;
    TRY(reader_peekSlice(reader, 4, &slice));

    *out = bitcastU32_LE(slice.slice);
    return reader_skip(reader, 4);
}

Error reader_takeI32_LE(Reader* reader, int32_t* out)
{
    Slice slice;
    TRY(reader_peekSlice(reader, 4, &slice));

    *out = bitcastI32_LE(slice.slice);
    return reader_skip(reader, 4);
}

Error reader_takeI24_LE(Reader* reader, Int24* out)
{
    Slice slice;
    TRY(reader_peekSlice(reader, 4, &slice));

    *out = bitcastI24_LE(slice.slice);
    return reader_skip(reader, 3);
}
