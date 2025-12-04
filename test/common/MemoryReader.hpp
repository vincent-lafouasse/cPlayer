#pragma once

#include <string>
#include <vector>

extern "C" {
#include "Reader.h"
}

using Byte = uint8_t;

class MemoryReader {
   private:
    std::vector<Byte> data_;
    size_t pos_;

   public:
    MemoryReader(const std::vector<Byte>& v) : data_(v), pos_(0) {}
    MemoryReader(const std::string& s) : data_(), pos_(0)
    {
        for (Byte b : s) {
            this->data_.push_back(b);
        }
    }

    size_t pos() const { return this->pos_; }
    size_t remaining() const { return this->data_.size() - this->pos_; }
    Byte* head() { return this->data_.data() + this->pos_; }

    Error peekSlice(size_t size, Slice* out)
    {
        if (size > this->remaining()) {
            return E_UnexpectedEOF;
        }

        *out = {.slice = this->head(), .len = size};
        return NoError;
    }

    Error peekInto(size_t size, Byte* out)
    {
        Slice slice;
        Error err = this->peekSlice(size, &slice);
        if (err != NoError) {
            return err;
        }

        memcpy(out, slice.slice, slice.len);
        return NoError;
    }

    Error skip(size_t size)
    {
        if (size > this->remaining()) {
            return E_UnexpectedEOF;
        } else {
            this->pos_ += size;
            return NoError;
        }
    }
};

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
