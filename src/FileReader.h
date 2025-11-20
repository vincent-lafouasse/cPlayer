#pragma once

#include <stddef.h>

#include "ints.h"

#define BUFFER_SIZE 64

typedef enum {
    Read_Ok,
    Read_Err,
    Read_Done,
} ReadResult;

const char* rr_repr(ReadResult rr);

typedef struct {
    int fd;
    u8 buffer[BUFFER_SIZE];
    size_t head;
    size_t len;
} FileReader;

FileReader fr_open(const char* path);
void fr_close(FileReader* fr);

ReadResult fr_takeByte(FileReader* fr, u8* out);
ReadResult fr_peekByte(FileReader* fr, u8* out);

typedef enum {
    LittleEndian,
    BigEndian,
} Endianness;

ReadResult fr_peekU16LE(FileReader* fr, u16* out);
ReadResult fr_takeU16LE(FileReader* fr, u16* out);
