#include "FileReader.h"

#include <fcntl.h>
#include <unistd.h>

FileReader fr_new(const char* path)
{
    int fd = open(path, O_RDONLY);

    return (FileReader){
        .fd = fd, .head = 0, .len = 0, .buffer_size = BUFFER_SIZE};
}

void fr_close(FileReader* fr)
{
    if (fr->fd > 2) {
        close(fr->fd);
    }
    fr->fd = -1;
}

ReadResult fr_takeByte(FileReader* fr, u8* out)
{
    u8 c;
    ReadResult res = fr_peekByte(fr, &c);

    if (res == Read_Ok) {
        *out = c;
        fr->head += 1;
    }
    return res;
}

ReadResult fr_peekByte(FileReader* fr, u8* out)
{
    if (fr == NULL) {
        return Read_Err;
    }
    if (fr->len == 0 || fr->head == fr->len) {
        ssize_t bytesRead = read(fr->fd, fr->buffer, fr->buffer_size);
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
