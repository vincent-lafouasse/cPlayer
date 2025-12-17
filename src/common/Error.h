#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define TRY(func_call)                  \
    do {                                \
        Error __temp_err = (func_call); \
        if (!err_isOk(__temp_err)) {    \
            return __temp_err;          \
        }                               \
    } while (0)

#define TRY_CTX(func_call, ctxBits)                 \
    do {                                            \
        Error __temp_err = (func_call);             \
        if (!err_isOk(__temp_err)) {                \
            return err_addCtx(__temp_err, ctxBits); \
        }                                           \
    } while (0)

// u32 error context      most significant digits
// u16 error subcategory
// u16 error category     least significant digits
typedef struct {
    uint64_t bits;
} Error;

// will be cast to u16
typedef enum {
    E_NoError = 0,
    E_Read,
    E_Option,
    E_Codec,
    E_Wav,
    E_System,     // e.g. oom
    E_Portaudio,  // later
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
    EOpt_UnimplementedFlag,  // should not see prod
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

// constructors
Error err_Ok(void);
Error err_Err(ErrorCategory category, uint16_t subCategory);
Error err_addCtx(Error err, uint32_t context);
Error err_withCtx(ErrorCategory category,
                  uint16_t subCategory,
                  uint32_t context);

// accessors
uint16_t err_category(Error err);
bool err_isOk(Error err);
uint16_t err_subCategory(Error err);
uint32_t err_context(Error err);

#ifdef __cplusplus
}
#endif
