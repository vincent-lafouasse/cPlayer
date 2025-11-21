#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FileReader.h"

// or double, go nuts
#define FLOAT float

typedef struct {
    uint8_t nChannels;
    uint32_t sampleRate;
    uint32_t size;
} WavHeader;

WavHeader parseWavHeader(FileReader* reader);

typedef struct {
    uint8_t nChannels;
    uint32_t sampleRate;
    uint32_t size;
    FLOAT* left;
    FLOAT* right;
} AudioData;

AudioData audio_parseWav(FileReader* reader);
AudioData audio_dumpCsv(const AudioData* audio);

ReadResult readFourCC(FileReader* reader, uint8_t* out)
{
    ReadResult res;

    for (size_t i = 0; i < 4; ++i) {
        res = fr_takeByte(reader, out + i);
        if (res != Read_Ok) {
            return res;
        }
    }
    return Read_Ok;
}

int main(void)
{
    const char* path = "./wav/f1_32bit.wav";
    FileReader reader = fr_open(path);
    if (!fr_isOpened(&reader)) {
        fprintf(stderr, "Failed to open file %s\n", path);
        exit(1);
    }

    uint8_t chunkID[4];
    assert(readFourCC(&reader, chunkID) == Read_Ok);
    assert(strncmp((const char*)chunkID, "RIFF", 4) == 0);

    fprintf(stderr, "ok\n");
    fr_close(&reader);
}
