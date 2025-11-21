#include "FileReader.h"

#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

static const size_t buffer_size = FILE_READER_BUFFER_SIZE;

FileReader fr_open(const char* path)
{
    const int fd = open(path, O_RDONLY);

    return (FileReader){.fd = fd, .head = 0, .len = 0};
}

void fr_close(FileReader* fr)
{
    if (fr->fd > 2) {
        close(fr->fd);
    }
    fr->fd = -1;
}

bool fr_isOpened(const FileReader* fr)
{
    return fr && fr->fd != -1;
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
    const ssize_t bytesRead =
        read(r->fd, r->buffer + r->len, buffer_size - r->len);

    if (bytesRead < 0) {
        return Read_Err;
    } else if (bytesRead == 0) {
        return Read_Done;
    }

    r->len += bytesRead;
    return Read_Ok;
}

ReadResult fr_peekSlice(FileReader* fr, uint8_t* out, size_t sz)
{
    assert(sz < buffer_size);

    if (fr->len - fr->head < sz) {
        fr_reseatHead(fr);
        const ReadResult res = fr_fillRemaining(fr);
        if (res != Read_Ok) {
            return res;
        }
    }
    if (fr->len < sz) {
        return Read_Err;
    }

    // a correct read is assured here
    for (size_t i = 0; i < sz; ++i) {
        out[i] = fr->buffer[fr->head + i];
    }
    return Read_Ok;
}

ReadResult fr_takeSlice(FileReader* fr, uint8_t* out, size_t sz)
{
    const ReadResult res = fr_peekSlice(fr, out, sz);
    if (res == Read_Ok) {
        fr->head += sz;
    }
    return res;
}

ReadResult fr_takeByte(FileReader* fr, uint8_t* out)
{
    uint8_t byte;
    const ReadResult res = fr_peekByte(fr, &byte);

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
        const ssize_t bytesRead = read(fr->fd, fr->buffer, buffer_size);
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
