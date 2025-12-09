#pragma once

#include <string>
#include <vector>

#include "Reader.h"

using Byte = uint8_t;

class MemoryReader {
   private:
    std::vector<Byte> data;
    size_t pos;

   public:
    MemoryReader(const std::vector<Byte>& v);
    MemoryReader(const std::string& s);

    LibStream_ReadStatus peekSlice(size_t size, Slice* out) const;
    LibStream_ReadStatus skip(size_t size);
};

LibStream_ReadStatus memoryReaderPeekSlice(Reader* reader,
                                           size_t n,
                                           Slice* out);
LibStream_ReadStatus memoryReaderSkip(Reader* reader, size_t n);
Reader memoryReaderInterface(MemoryReader* memoryReader);
