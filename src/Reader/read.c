#include "Reader.h"

#include <string.h>

#include "bitcast.h"
#include "int24.h"

Error reader_takeSlice(Reader* reader, size_t n, Slice* out)
{
    Error err = reader->peekSlice(reader, n, out);

    if (err_isOk(err)) {
        TRY(reader->skip(reader, n));
    }
    return err;
}

Error reader_peekFourCC(Reader* reader, uint8_t* out)
{
    Slice slice;
    TRY(reader->peekSlice(reader, 4, &slice));

    memcpy(out, slice.slice, 4);
    return err_Ok();
}

Error reader_takeFourCC(Reader* reader, uint8_t* out)
{
    TRY(reader_peekFourCC(reader, out));

    return reader->skip(reader, 4);
}

Error reader_takeU16_LE(Reader* reader, uint16_t* out)
{
    Slice slice;
    TRY(reader->peekSlice(reader, 2, &slice));

    *out = bitcastU16_LE(slice.slice);
    return reader->skip(reader, 2);
}

Error reader_takeI16_LE(Reader* reader, int16_t* out)
{
    Slice slice;
    TRY(reader->peekSlice(reader, 2, &slice));

    *out = bitcastI16_LE(slice.slice);
    return reader->skip(reader, 2);
}

Error reader_takeU32_LE(Reader* reader, uint32_t* out)
{
    Slice slice;
    TRY(reader->peekSlice(reader, 4, &slice));

    *out = bitcastU32_LE(slice.slice);
    return reader->skip(reader, 4);
}

Error reader_takeI32_LE(Reader* reader, int32_t* out)
{
    Slice slice;
    TRY(reader->peekSlice(reader, 4, &slice));

    *out = bitcastI32_LE(slice.slice);
    return reader->skip(reader, 4);
}

Error reader_takeI24_LE(Reader* reader, Int24* out)
{
    Slice slice;
    TRY(reader->peekSlice(reader, 4, &slice));

    *out = bitcastI24_LE(slice.slice);
    return reader->skip(reader, 3);
}
