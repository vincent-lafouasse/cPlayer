#pragma once

#include <stdbool.h>
#include <stdint.h>

#define TRY64(func_call)                  \
    do {                                  \
        Error64 __temp_err = (func_call); \
        if (!err_isOk(__temp_err)) {      \
            return __temp_err;            \
        }                                 \
    } while (0)

#define TRY64_CTX(func_call, ctxBits)               \
    do {                                            \
        Error64 __temp_err = (func_call);           \
        if (!err_isOk(__temp_err)) {                \
            return err_addCtx(__temp_err, ctxBits); \
        }                                           \
    } while (0)

// u32 error context      most significant digits
// u16 error subcategory
// u16 error category     least significant digits
typedef uint64_t Error64;

// will be cast to u16
typedef enum {
    E64_NoError = 0,
    E64_Read,
    E64_Option,
    E64_Codec,
    E64_Wav,
    E64_System,     // e.g. oom
    E64_Portaudio,  // later
} ErrorCategory;

// those enums will probably move to their respective modules
// or not
typedef enum {
    ERd_OpenFailed,
    ERd_ReadFailed,
    ERd_UnexpectedEOF,
} ReadError;

typedef enum {
    EOpt_BadUsage,
    EOpt_UnknownFlag,
    EOpt_HelpRequested,
} OptionError;

typedef enum {
    ESys_OutOfMemory,
} SystemError;

typedef enum {
    ECdc_UnsupportedCodec,
    ECdc_UnsupportedChannelLayout,
    ECdc_AbsurdSampleRate,
} CodecError;

typedef enum {
    EWav_UnknownFourCC,
    EWav_UnsupportedSampleFormat,
    EWav_InvalidBitDepth,
    EWav_BlockAlignMismatch,
    EWav_FormatChunkTooSmall,
    EWav_ExtensionSizeMismatch,
    EWav_UnsupportedBitDepth,
    EWav_UnknownSampleFormat,
} WavError;

// constructors
static inline Error64 err_Ok(void)
{
    return 0ull;
}

static inline Error64 err_Err(ErrorCategory category, uint16_t subCategory)
{
    return (uint64_t)category | ((uint64_t)subCategory << 16);
}

static inline Error64 err_addCtx(Error64 err, uint32_t context)
{
    return err | ((uint64_t)context << 32);
}

static inline Error64 err_withCtx(ErrorCategory category,
                                  uint16_t subCategory,
                                  uint32_t context)
{
    return err_addCtx(err_Err(category, subCategory), context);
}

// accessors
static inline uint16_t err_category(Error64 err)
{
    return err & 0xffff;
}

static inline bool err_isOk(Error64 err)
{
    return err_category(err) == 0;
}

static inline uint16_t err_subCategory(Error64 err)
{
    return (err >> 16) & 0xffff;
}

static inline uint32_t err_context(Error64 err)
{
    return (err >> 32) & 0xffffffff;
}
