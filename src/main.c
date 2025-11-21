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

int main(void)
{
    const char* path = "./wav/f1_32bit.wav";
    FileReader reader = fr_open(path);
    if (!fr_isOpened(&reader)) {
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
