#include "Reader.h"

#include <string.h>

#include "bitcast.h"
#include "int24.h"

Error64 reader_takeSlice(Reader* reader, size_t n, Slice* out)
{
    Error64 err = reader->peekSlice(reader, n, out);

    if (err_isOk(err)) {
        TRY64(reader->skip(reader, n));
    }
    return err;
}

Error64 reader_peekFourCC(Reader* reader, uint8_t* out)
{
    Slice slice;
    TRY64(reader->peekSlice(reader, 4, &slice));

    memcpy(out, slice.slice, 4);
    return err_Ok();
}

Error64 reader_takeFourCC(Reader* reader, uint8_t* out)
{
    TRY64(reader_peekFourCC(reader, out));

    return reader->skip(reader, 4);
}

Error64 reader_takeU16_LE(Reader* reader, uint16_t* out)
{
    Slice slice;
    TRY64(reader->peekSlice(reader, 2, &slice));

    *out = bitcastU16_LE(slice.slice);
    return reader->skip(reader, 2);
}

Error64 reader_takeI16_LE(Reader* reader, int16_t* out)
{
    Slice slice;
    TRY64(reader->peekSlice(reader, 2, &slice));

    *out = bitcastI16_LE(slice.slice);
    return reader->skip(reader, 2);
}

Error64 reader_takeU32_LE(Reader* reader, uint32_t* out)
{
    Slice slice;
    TRY64(reader->peekSlice(reader, 4, &slice));

    *out = bitcastU32_LE(slice.slice);
    return reader->skip(reader, 4);
}

Error64 reader_takeI32_LE(Reader* reader, int32_t* out)
{
    Slice slice;
    TRY64(reader->peekSlice(reader, 4, &slice));

    *out = bitcastI32_LE(slice.slice);
    return reader->skip(reader, 4);
}

Error64 reader_takeI24_LE(Reader* reader, Int24* out)
{
    Slice slice;
    TRY64(reader->peekSlice(reader, 4, &slice));

    *out = bitcastI24_LE(slice.slice);
    return reader->skip(reader, 3);
}
