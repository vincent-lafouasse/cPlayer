#include <stdlib.h>

#include "FileReader.h"

#include "log.h"
#include "wav.h"

int main(void)
{
    const char* path = "./wav/f1_24bit.wav";
    FileReader reader = fr_open(path);
    if (!fr_isOpened(&reader)) {
        logFn("Failed to open file %s\n", path);
        exit(1);
    }

    WavHeader header = readWavHeader(&reader);
    logWavHeader(&header, path);

    logFn("ok\n");
    fr_close(&reader);
}
