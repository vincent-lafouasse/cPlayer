#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FileReader.h"

int main(void)
{
    const char* path = "./wav/f1_32bit.wav";
    FileReader reader = fr_open(path);
    if (reader.fd == -1) {
        printf("Failed to open file %s\n", path);
        exit(1);
    }

    uint8_t chunkID[4];
    fr_takeByte(&reader, chunkID);
    fr_takeByte(&reader, chunkID + 1);
    fr_takeByte(&reader, chunkID + 2);
    fr_takeByte(&reader, chunkID + 3);
    assert(strncmp((const char*)chunkID, "RIFF", 4) == 0);

    fr_close(&reader);
}
