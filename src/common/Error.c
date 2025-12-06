#include "Error.h"

// constructors
Error err_Ok(void)
{
    return (Error){.bits = 0ull};
}

Error err_Err(ErrorCategory category, uint16_t subCategory)
{
    return (Error){.bits = (uint64_t)category | ((uint64_t)subCategory << 16)};
}

Error err_addCtx(Error err, uint32_t context)
{
    return (Error){.bits = err.bits | ((uint64_t)context << 32)};
}

Error err_withCtx(ErrorCategory category,
                  uint16_t subCategory,
                  uint32_t context)
{
    return err_addCtx(err_Err(category, subCategory), context);
}

// accessors
uint16_t err_category(Error err)
{
    return err.bits & 0xffff;
}

bool err_isOk(Error err)
{
    return err_category(err) == 0;
}

uint16_t err_subCategory(Error err)
{
    return (err.bits >> 16) & 0xffff;
}

uint32_t err_context(Error err)
{
    return (err.bits >> 32) & 0xffffffff;
}
