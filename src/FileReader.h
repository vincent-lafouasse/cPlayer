#pragma once

#include <stddef.h>

#define BUFFER_SIZE 64

typedef enum {
    Ok,
    Err,
    Done,
} ReadResult;

const char* rr_repr(ReadResult rr);

typedef struct {
    int fd;
    char buffer[BUFFER_SIZE];
    size_t head;
    size_t len;
    const size_t buffer_size;
} FileReader;

FileReader fr_new(const char* path);
void fr_close(FileReader* fr);
ReadResult fr_takeByte(FileReader* fr, char* out);
ReadResult fr_peekByte(FileReader* fr, char* out);
