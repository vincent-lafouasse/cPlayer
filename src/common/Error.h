#pragma once

#include <stddef.h>
#include <stdint.h>

#define TRY(func_call)                  \
    do {                                \
        Error __temp_err = (func_call); \
        if (__temp_err != NoError) {    \
            return __temp_err;          \
        }                               \
    } while (0)

#define TRY_CTX(func_call)                 \
    do {                                   \
        ErrorCtx __temp_err = (func_call); \
        if (__temp_err.err != NoError) {   \
            return __temp_err;             \
        }                                  \
    } while (0)

#define TRY_CTX_ADD(func_call, contextBits)               \
    do {                                                  \
        Error __temp_err = (func_call);                   \
        if (__temp_err != NoError) {                      \
            return ErrorCtx_Err(__temp_err, contextBits); \
        }                                                 \
    } while (0)

typedef enum {
    NoError = 0,
    E_OOM,

    E_Bad_Usage,
    E_Unknown_Flag,
    E_HelpRequested,

    E_FailedRead,
    E_UnexpectedEOF,

    E_Codec_UnsupportedCodec,
    E_Codec_UnsupportedChannelLayout,
    E_Codec_AbsurdSampleRate,

    E_Wav_UnknownFourCC,
    E_Wav_UnsupportedSampleFormat,
    E_Wav_InvalidBitDepth,
    E_Wav_BlockAlignMismatch,
    E_Wav_FormatChunkTooSmall,
    E_Wav_ExtensionSizeMismatch,
    E_Wav_UnsupportedBitDepth,
    E_Wav_UnknownSampleFormat,

    E_Unimplemented,
} Error;

const char* errorRepr(Error e);

typedef struct {
    Error err;
    uint32_t context;
} ErrorCtx;

static inline ErrorCtx ErrorCtx_Ok(void)
{
    return (ErrorCtx){.err = NoError, .context = 0};
}

static inline ErrorCtx ErrorCtx_Err(Error err, uint32_t ctx)
{
    return (ErrorCtx){.err = err, .context = ctx};
}
