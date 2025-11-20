#include <stdio.h>
#include <stdlib.h>

#include "FileReader.h"

int main(void)
{
    const char* path = "./src/main.c";
    FileReader reader = fr_new(path);
    if (reader.fd == -1) {
        printf("Failed to open file %s\n", path);
        exit(1);
    }

    char c;
    ReadResult res;
    while ((res = fr_takeByte(&reader, &c)) == Read_Ok) {
        printf("%c", c);
    }
    printf("%s\n", rr_repr(res));
    fr_close(&reader);
}
