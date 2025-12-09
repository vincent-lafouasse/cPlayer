#include "MemoryReader.hpp"

#include <cstring>

#include "Reader.h"

static LibStream_ReadStatus ls_Ok()
{
    return LibStream_ReadStatus_Ok;
}

static LibStream_ReadStatus ls_EOF()
{
    return LibStream_ReadStatus_UnexpectedEOF;
}

static bool ls_isOk(LibStream_ReadStatus err)
{
    return err == ls_Ok();
}

MemoryReader::MemoryReader(const std::vector<Byte>& v) : data(v), pos(0) {}

MemoryReader::MemoryReader(const std::string& s) : data(), pos(0)
{
    for (Byte b : s) {
        this->data.push_back(b);
    }
}

LibStream_ReadStatus MemoryReader::peekSlice(size_t size, Slice* out) const
{
    if (this->pos + size > this->data.size()) {
        return ls_EOF();
    }

    *out = {.slice = this->data.data() + this->pos, .len = size};
    return ls_Ok();
}

LibStream_ReadStatus MemoryReader::skip(size_t size)
{
    if (this->pos + size > this->data.size()) {
        return ls_EOF();
    } else {
        this->pos += size;
        return ls_Ok();
    }
}

LibStream_ReadStatus memoryReaderPeekSlice(Reader* reader, size_t n, Slice* out)
{
    MemoryReader* memoryReader = static_cast<MemoryReader*>(reader->ctx);

    return memoryReader->peekSlice(n, out);
}

LibStream_ReadStatus memoryReaderSkip(Reader* reader, size_t n)
{
    MemoryReader* memoryReader = static_cast<MemoryReader*>(reader->ctx);

    LibStream_ReadStatus err = memoryReader->skip(n);
    if (!ls_isOk(err)) {
        return err;
    } else {
        reader->offset += n;
        return ls_Ok();
    }
}

Reader memoryReaderInterface(MemoryReader* memoryReader)
{
    return {
        .ctx = static_cast<void*>(memoryReader),
        .peekSlice = memoryReaderPeekSlice,
        .skip = memoryReaderSkip,
        .offset = 0,
    };
}
