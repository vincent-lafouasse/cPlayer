#include "MemoryReader.hpp"

#include <cstring>
#include "Error64.h"

MemoryReader::MemoryReader(const std::vector<Byte>& v) : data(v), pos(0) {}

MemoryReader::MemoryReader(const std::string& s) : data(), pos(0)
{
    for (Byte b : s) {
        this->data.push_back(b);
    }
}

Error64 MemoryReader::peekSlice(size_t size, Slice* out) const
{
    if (this->pos + size > this->data.size()) {
        return err_Err(E64_Read, ERd_UnexpectedEOF);
    }

    *out = {.slice = this->data.data() + this->pos, .len = size};
    return err_Ok();
}

Error64 MemoryReader::peekInto(size_t size, Byte* out) const
{
    Slice slice;
    Error64 err = this->peekSlice(size, &slice);
    if (!err_isOk(err)) {
        return err;
    }

    memcpy(out, slice.slice, slice.len);
    return err_Ok();
}

Error64 MemoryReader::skip(size_t size)
{
    if (this->pos + size > this->data.size()) {
        return err_Err(E64_Read, ERd_UnexpectedEOF);
    } else {
        this->pos += size;
        return err_Ok();
    }
}

Error64 memoryReaderPeekSlice(Reader* reader, size_t n, Slice* out)
{
    MemoryReader* memoryReader = static_cast<MemoryReader*>(reader->ctx);

    return memoryReader->peekSlice(n, out);
}

Error64 memoryReaderPeekInto(Reader* reader, size_t n, uint8_t* out)
{
    MemoryReader* memoryReader = static_cast<MemoryReader*>(reader->ctx);

    return memoryReader->peekInto(n, out);
}

Error64 memoryReaderSkip(Reader* reader, size_t n)
{
    MemoryReader* memoryReader = static_cast<MemoryReader*>(reader->ctx);

    Error64 err = memoryReader->skip(n);
    if (!err_isOk(err)) {
        return err;
    } else {
        reader->offset += n;
        return err_Ok();
    }
}

Reader memoryReaderInterface(MemoryReader* memoryReader)
{
    return {
        .ctx = static_cast<void*>(memoryReader),
        .peekSlice = memoryReaderPeekSlice,
        .peekInto = memoryReaderPeekInto,
        .skip = memoryReaderSkip,
        .offset = 0,
    };
}
