#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define BUFFER_SIZE 64

typedef enum {
    Read_Ok,
    Read_Err,
    Read_Done,
} ReadResult;

const char* rr_repr(ReadResult rr);

typedef struct {
    int fd;
    uint8_t buffer[BUFFER_SIZE];
    size_t head;
    size_t len;
} FileReader;

FileReader fr_open(const char* path);
void fr_close(FileReader* fr);
bool fr_isOpened(const FileReader* fr);

ReadResult fr_peekByte(FileReader* fr, uint8_t* out);
ReadResult fr_takeByte(FileReader* fr, uint8_t* out);

ReadResult fr_peekU16LE(FileReader* fr, uint16_t* out);
ReadResult fr_takeU16LE(FileReader* fr, uint16_t* out);
