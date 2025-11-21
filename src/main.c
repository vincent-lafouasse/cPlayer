#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "FileReader.h"
#include "int24.h"

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

ReadResult readI24AsI32LE(FileReader* reader, int32_t* out)
{
    uint8_t bytes[3];
    for (size_t i = 0; i < 4; ++i) {
        const ReadResult res = fr_takeByte(reader, bytes + i);
        if (res != Read_Ok) {
            return res;
        }
    }
    *out = i24_asI32((Int24){bytes[2], bytes[1], bytes[0]});
    return Read_Ok;
}

int main(void)
{
    const char* path = "./wav/f1_24bit.wav";
    FileReader reader = fr_open(path);
    if (!fr_isOpened(&reader)) {
        logFn("Failed to open file %s\n", path);
        exit(1);
    }

    uint8_t masterChunkID[5] = {0};
    assert(readFourCC(&reader, masterChunkID) == Read_Ok);
    assert(strncmp((const char*)masterChunkID, "RIFF", 4) == 0);
    logFn("master chunk ID:\t%s\n", masterChunkID);

    uint32_t masterChunkSize;
    assert(fr_takeU32LE(&reader, &masterChunkSize) == Read_Ok);
    logFn("chunk size:\t\t%u bytes\n", masterChunkSize);

    uint8_t wavChunkID[5] = {0};
    assert(readFourCC(&reader, wavChunkID) == Read_Ok);
    assert(strncmp((const char*)wavChunkID, "WAVE", 4) == 0);
    logFn("wav chunk ID:\t\t%s\n", wavChunkID);

    uint8_t fmtChunkID[5] = {0};
    assert(readFourCC(&reader, fmtChunkID) == Read_Ok);
    assert(strncmp((const char*)fmtChunkID, "fmt ", 4) == 0);
    logFn("fmt chunk ID:\t\t%s\n", fmtChunkID);

    uint32_t fmtChunkSize;
    assert(fr_takeU32LE(&reader, &fmtChunkSize) == Read_Ok);
    logFn("format chunk size:\t%u bytes\n", fmtChunkSize);

    uint16_t waveFormat;
    assert(fr_takeU16LE(&reader, &waveFormat) == Read_Ok);
    logFn("wave format:\t\t0x%04:x\n", waveFormat);
    if (waveFormat != WAVE_FORMAT_PCM) {
        logFn("\nError:\n\tUnsupported wave format\n");
        logFn("Only PCM is supported for now\n");
        exit(1);
    }

    uint16_t nChannels;
    assert(fr_takeU16LE(&reader, &nChannels) == Read_Ok);
    logFn("n. channels:\t\t%x\n", nChannels);

    logFn("ok\n");
    fr_close(&reader);
}
