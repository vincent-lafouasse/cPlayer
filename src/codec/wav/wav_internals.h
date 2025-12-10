#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wav.h"

#include "Error.h"

#define WAVE_FORMAT_PCM 0x0001
#define WAVE_FORMAT_ADPCM 0x0002
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#define WAVE_FORMAT_ALAW 0x0006
#define WAVE_FORMAT_MULAW 0x0007
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE

// ----------- Main interface

typedef enum {
    SampleFormat_Unsigned8,
    SampleFormat_Signed16,
    SampleFormat_Signed24,
    SampleFormat_Signed32,
    SampleFormat_Float32,
    SampleFormat_Float64,
    SampleFormat_ADPCM,  // for IMA-ADPCM blocks
    SampleFormat_MULAW,  // μ-law
    SampleFormat_ALAW,   // A-law
} SampleFormat;

typedef struct {
    uint16_t formatTag;
    uint16_t nChannels;
    uint32_t sampleRate;
    SampleFormat sampleFormat;
    uint16_t blockAlign;  // bytes per sample block (nChannels * bytesPerSample
                          // or ADPCM block)
    uint32_t nFrames;
    uint16_t bitDepth;
    uint16_t adpcmBlockSize;  // only for ADPCM, else 0
} WavFormatInfo;

// gather what's needed to deserialize the data segment
// places the head at the beginning of the data payload
Error readWavFormatInfo(Reader* reader, WavFormatInfo* out);

// read the actual data
Error readWavData(Reader* reader,
                  const WavFormatInfo* format,
                  AudioBuffer* out);

// ----------- Implementation

// RIFF chunk navigation
typedef struct {
    uint8_t fourcc[5];  // yes i waste a byte for easier logging
    uint32_t size;
} RiffChunkHeader;

Error skipChunkUntil(Reader* reader, const char* expectedId);
Error getToFormatChunk(Reader* reader);

// read/validate format chunk
typedef struct {
    uint32_t size;
    uint16_t formatTag;
    uint16_t nChannels;
    uint32_t sampleRate;
    uint32_t bytesPerSecond;
    uint16_t blockAlign;
    uint16_t bitDepth;
    uint16_t extensionSize;
    // extension
    uint16_t validBitsPerSample;
    uint32_t channelMask;
    uint8_t subFormat[16];
} WavFormatChunk;

Error readFormatChunk(Reader* reader, WavFormatChunk* out);
Error validateWavFormatChunk(const WavFormatChunk* format);
Error parseFormatChunk(const WavFormatChunk* chunk, WavFormatInfo* out);
Error checkFormatSupport(
    const WavFormatInfo* format);  // reject codecs i don't support

// dispatches between the different formats and outputs a single sample
Error readSample(Reader* reader, const WavFormatInfo* format, float* out);

// debug utils
const char* sampleFormatRepr(SampleFormat fmt);
void logWavFormatInfo(const WavFormatInfo* format);

static inline Error err_fromIo(LibStream_ReadStatus status)
{
    switch (status) {
        case LibStream_ReadStatus_Ok:
            return err_Ok();
        case LibStream_ReadStatus_ReadFailed:
            return err_Err(E_Read, ERd_ReadFailed);
        case LibStream_ReadStatus_UnexpectedEOF:
            return err_Err(E_Read, ERd_UnexpectedEOF);
    }
}

#define TRY_IO(func_call)                           \
    do {                                            \
        Error __temp_err = (err_fromIo(func_call)); \
        if (!err_isOk(__temp_err)) {                \
            return __temp_err;                      \
        }                                           \
    } while (0)

#define TRY_IO_CTX(func_call, ctxBits)              \
    do {                                            \
        Error __temp_err = (err_fromIo(func_call)); \
        if (!err_isOk(__temp_err)) {                \
            return err_addCtx(__temp_err, ctxBits); \
        }                                           \
    } while (0)

#define DUMP_PREFIX "./build/dump_"
#define DUMP_SUFFIX ".csv"

void dumpIntCsv(const int* data, unsigned sz, const char* path);
void dumpFloatCsv(const float* data, unsigned sz, const char* path);

#ifdef __cplusplus
}
#endif
