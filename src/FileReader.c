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

ReadResult fr_takeByte(FileReader* fr, char* out)
{
    if (fr == NULL) {
        return Err;
    }
    if (fr->len == 0 || fr->head == fr->len) {
        ssize_t bytesRead = read(fr->fd, fr->buffer, fr->buffer_size);
        if (bytesRead < 0) {
            return Err;
        } else if (bytesRead == 0) {
            return Done;
        }
        fr->len = bytesRead;
        fr->head = 0;
    }
    *out = fr->buffer[fr->head++];
    return Ok;
}

const char* rr_repr(ReadResult rr)
{
    switch (rr) {
        case Ok:
            return "Ok";
        case Err:
            return "Err";
        case Done:
            return "Done";
    }
    return NULL;
}
