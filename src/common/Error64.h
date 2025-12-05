#pragma once

#include <stdbool.h>
#include <stdint.h>

// u32 error context      most significant digits
// u16 error subcategory
// u16 error category     least significant digits
typedef uint64_t Error64;

// will be cast to u16
typedef enum {
    NoError = 0,
    E_Read,
    E_Codec,
    E_System,  // e.g. oom
    E_Portaudio,
} ErrorCategory;

// constructors
Error64 err_Ok(void)
{
    return (Error64){0};
}

Error64 err_Err(ErrorCategory category, uint16_t subCategory)
{
    return (uint64_t)category | ((uint64_t)subCategory << 16);
}

Error64 err_addCtx(Error64 err, uint32_t context)
{
    return err | ((uint64_t)context << 32);
}

Error64 err_withCtx(ErrorCategory category,
                    uint16_t subCategory,
                    uint32_t context)
{
    return err_addCtx(err_Err(category, subCategory), context);
}

// accessors
uint16_t err_category(Error64 err)
{
    return err & 0xffff;
}

bool err_isOk(Error64 err)
{
    return err_category(err) == 0;
}

uint16_t err_subCategory(Error64 err)
{
    return (err >> 16) & 0xffff;
}

uint32_t err_context(Error64 err)
{
    return (err >> 32) & 0xffffffff;
}
