#include "MemoryReader.hpp"

#include <cstring>

MemoryReader::MemoryReader(const std::vector<Byte>& v) : data(v), pos(0) {}

MemoryReader::MemoryReader(const std::string& s) : data(), pos(0)
{
    for (Byte b : s) {
        this->data.push_back(b);
    }
}

Error MemoryReader::peekSlice(size_t size, Slice* out) const
{
    if (this->pos + size > this->data.size()) {
        return E_UnexpectedEOF;
    }

    *out = {.slice = this->data.data() + this->pos, .len = size};
    return NoError;
}

Error MemoryReader::peekInto(size_t size, Byte* out) const
{
    Slice slice;
    Error err = this->peekSlice(size, &slice);
    if (err != NoError) {
        return err;
    }

    memcpy(out, slice.slice, slice.len);
    return NoError;
}

Error MemoryReader::skip(size_t size)
{
    if (this->pos + size > this->data.size()) {
        return E_UnexpectedEOF;
    } else {
        this->pos += size;
        return NoError;
    }
}

Error memoryReaderPeekSlice(Reader* reader, size_t n, Slice* out)
{
    MemoryReader* memoryReader = static_cast<MemoryReader*>(reader->ctx);

    return memoryReader->peekSlice(n, out);
}

Error memoryReaderPeekInto(Reader* reader, size_t n, uint8_t* out)
{
    MemoryReader* memoryReader = static_cast<MemoryReader*>(reader->ctx);

    return memoryReader->peekInto(n, out);
}

Error memoryReaderSkip(Reader* reader, size_t n)
{
    MemoryReader* memoryReader = static_cast<MemoryReader*>(reader->ctx);

    Error err = memoryReader->skip(n);
    if (err != NoError) {
        return err;
    } else {
        reader->offset += n;
        return NoError;
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
