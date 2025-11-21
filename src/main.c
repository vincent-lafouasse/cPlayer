#include <stdlib.h>

#include "FileReader.h"

#include "log.h"
#include "wav.h"

int main(void)
{
    const char* path = "./wav/f1_24bit.wav";
    logFn("-----Reading file\t%s-----\n", path);
    FileReader reader = fr_open(path);
    if (!fr_isOpened(&reader)) {
        logFn("Failed to open file %s\n", path);
        exit(1);
    }

    Header header = readWavHeader(&reader);
    logHeader(&header, path);

    logFn("ok\n");
    fr_close(&reader);
}
