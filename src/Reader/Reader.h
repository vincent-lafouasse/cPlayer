#pragma once

#include <stdbool.h>
#include <string.h>

#include "FileReader.h"
#include "FixedBufferReader.h"

typedef struct Reader Reader;

// true means a complete read has happened
// false means partial read or read error
typedef bool (*ReadFn)(Reader*, uint8_t*, size_t);
typedef bool (*SkipFn)(Reader*, size_t);

struct Reader {
    void* ctx;
    ReadFn read;
    SkipFn skip;
    size_t offset;
};

Reader reader_fromFileReader(FileReader* fileReader);
Reader readerFromFixedBufferReader(FixedBufferReader* fbReader);
