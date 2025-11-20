#include <stdio.h>
#include <stdlib.h>

#include "FileReader.h"

int main(void)
{
    const char* path = "./src/main.c";
    FileReader reader = fr_open(path);
    if (reader.fd == -1) {
        printf("Failed to open file %s\n", path);
        exit(1);
    }

    uint8_t byte;
    ReadResult res;
    while ((res = fr_takeByte(&reader, &byte)) == Read_Ok) {
        printf("%c", byte);
    }
    printf("%s\n", rr_repr(res));
    fr_close(&reader);
}
