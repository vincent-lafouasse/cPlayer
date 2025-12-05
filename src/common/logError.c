#include "Error64.h"
#include "log.h"

#include <assert.h>

#include "options/Options.h"

static void logSystemError(Error64 err, const ErrorLogCtx* ctx)
{
    assert(err_category(err) == E64_System);

    const SystemError sub = err_subCategory(err);
    const uint32_t context = err_context(err);

    (void)ctx;
    (void)context;

    switch (sub) {
        case ESys_OutOfMemory:
            logFn(LogLevel_Error, "Out of memory\n");
            break;
        default:
            logFn(LogLevel_Error, "Unknown system error %u\n", sub);
            break;
    }
}

static void logOptionError(Error64 err, const ErrorLogCtx* ctx)
{
    assert(err_category(err) == E64_Option);

    const OptionError sub = err_subCategory(err);
    const uint32_t context = err_context(err);
    const Options* options = ctx->options;

    switch (sub) {
        case EOpt_BadUsage:
            logFn(LogLevel_Error, "Bad usage\n");
            logFn(LogLevel_Error, "Usage: %s [OPTIONS] <input>\n",
                  ctx->argv[0]);
            break;
        case EOpt_UnknownFlag:
            logFn(LogLevel_Error, "Unknown flag %s\n", ctx->argv[context]);
            break;
        case EOpt_HelpRequested:
            logFn(LogLevel_Info,
                  "The program should now print the help message and exit\n");
            break;
    }
}

void logError(Error64 err, const ErrorLogCtx* ctx);
