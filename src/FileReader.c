#include "FileReader.h"

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

static const size_t _buffer_size = BUFFER_SIZE;

FileReader fr_open(const char* path)
{
    int fd = open(path, O_RDONLY);

    return (FileReader){.fd = fd, .head = 0, .len = 0};
}

void fr_close(FileReader* fr)
{
    if (fr->fd > 2) {
        close(fr->fd);
    }
    fr->fd = -1;
}

static void fr_reseatHead(FileReader* r)
{
    const size_t newLen = r->len - r->head;
    const uint8_t* head = r->buffer + r->head;
    memcpy(r->buffer, head, newLen);
    r->head = 0;
    r->len = newLen;
}

static ReadResult fr_fillRemaining(FileReader* r)
{
    ssize_t bytesRead = read(r->fd, r->buffer + r->len, _buffer_size - r->len);

    if (bytesRead < 0) {
        return Read_Err;
    } else if (bytesRead == 0) {
        return Read_Done;
    }

    r->len += bytesRead;
    return Read_Ok;
}

ReadResult fr_peekU16LE(FileReader* fr, uint16_t* out)
{
    if (fr->len - fr->head < 2) {
        fr_reseatHead(fr);
        ReadResult res = fr_fillRemaining(fr);
        if (res == Read_Err) {
            return Read_Err;
        }
    }
    if (fr->len < 2) {
        return Read_Err;
    }

    uint16_t lowByte = fr->buffer[0];
    uint16_t highByte = fr->buffer[1];
    *out = lowByte + (highByte << 8);
    return Read_Ok;
}

ReadResult fr_takeU16LE(FileReader* fr, uint16_t* out)
{
    ReadResult res = fr_peekU16LE(fr, out);
    if (res == Read_Ok) {
        fr->head += 2;
    }
    return res;
}

ReadResult fr_takeByte(FileReader* fr, uint8_t* out)
{
    uint8_t byte;
    ReadResult res = fr_peekByte(fr, &byte);

    if (res == Read_Ok) {
        *out = byte;
        fr->head += 1;
    }
    return res;
}

ReadResult fr_peekByte(FileReader* fr, uint8_t* out)
{
    if (fr == NULL) {
        return Read_Err;
    }
    if (fr->len == 0 || fr->head == fr->len) {
        ssize_t bytesRead = read(fr->fd, fr->buffer, _buffer_size);
        if (bytesRead < 0) {
            return Read_Err;
        } else if (bytesRead == 0) {
            return Read_Done;
        }
        fr->len = bytesRead;
        fr->head = 0;
    }
    *out = fr->buffer[fr->head];
    return Read_Ok;
}

const char* rr_repr(ReadResult rr)
{
    switch (rr) {
        case Read_Ok:
            return "Ok";
        case Read_Err:
            return "Err";
        case Read_Done:
            return "Done";
    }
    return NULL;
}
