#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "FileReader.h"

#include "log.h"

#define WAVE_FORMAT_PCM 0x0001
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#define WAVE_FORMAT_ALAW 0x0006
#define WAVE_FORMAT_MULAW 0x0007
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE

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
    for (size_t i = 0; i < 4; ++i) {
        const ReadResult res = fr_takeByte(reader, out + i);
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
        logFn("Failed to open file %s\n", path);
        exit(1);
    }

    uint8_t chunkID[4];
    assert(readFourCC(&reader, chunkID) == Read_Ok);
    assert(strncmp((const char*)chunkID, "RIFF", 4) == 0);

    uint32_t chunkSize;
    assert(fr_takeU32LE(&reader, &chunkSize) == Read_Ok);
    logFn("chunk size: %u bytes\n", chunkSize);

    uint8_t wavChunkID[4];
    assert(readFourCC(&reader, wavChunkID) == Read_Ok);
    assert(strncmp((const char*)wavChunkID, "WAVE", 4) == 0);

    logFn("ok\n");
    fr_close(&reader);
}
