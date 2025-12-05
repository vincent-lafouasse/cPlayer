#include "Error64.h"
#include "log.h"

#include <assert.h>

#include "options/Options.h"

static void logReadError(Error64 err, const ErrorLogCtx* ctx)
{
    assert(err_category(err) == E64_Read);

    const ReadError sub = err_subCategory(err);
    const uint32_t context = err_context(err);

    (void)ctx;
    (void)context;

    switch (sub) {
        default:
            logFn(LogLevel_Error, "Unknown read error %u\n", sub);
            break;
    }
}

static void logOptionError(Error64 err, const ErrorLogCtx* ctx)
{
    assert(err_category(err) == E64_Option);

    const OptionError sub = err_subCategory(err);
    const uint32_t context = err_context(err);
    const Options* options = ctx->options;

    (void)options;

    switch (sub) {
        case EOpt_BadUsage:
            logFn(LogLevel_Error, "Bad usage\n");
            logFn(LogLevel_Error, "Usage: %s [OPTIONS] <input>\n",
                  ctx->argv[0]);
            break;
        case EOpt_UnknownFlag:
            logFn(LogLevel_Error, "Unknown flag %s\n", ctx->argv[context + 1]);
            break;
        case EOpt_HelpRequested:
            logFn(LogLevel_Info,
                  "The program should now print the help message and exit\n");
            break;
        default:
            logFn(LogLevel_Error, "Unknown option parsing error %u\n", sub);
            break;
    }
}

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

static void logCodecError(Error64 err, const ErrorLogCtx* ctx)
{
    assert(err_category(err) == E64_Codec);

    const CodecError sub = err_subCategory(err);
    const uint32_t context = err_context(err);

    (void)ctx;
    (void)context;

    switch (sub) {
        default:
            logFn(LogLevel_Error, "Unknown codec error %u\n", sub);
            break;
    }
}

static void logWavError(Error64 err, const ErrorLogCtx* ctx)
{
    assert(err_category(err) == E64_Wav);

    const WavError sub = err_subCategory(err);
    const uint32_t context = err_context(err);

    (void)ctx;
    (void)context;

    switch (sub) {
        default:
            logFn(LogLevel_Error, "Unknown wav decoding error %u\n", sub);
            break;
    }
}

void logError(Error64 err, const ErrorLogCtx* ctx)
{
    const ErrorCategory category = err_category(err);

    switch (category) {
        case E64_NoError:
            logFn(LogLevel_Info, "Everything good\n");
            break;
        case E64_Read:
            logReadError(err, ctx);
            break;
        case E64_Option:
            logOptionError(err, ctx);
            break;
        case E64_Codec:
            logCodecError(err, ctx);
            break;
        case E64_Wav:
            logWavError(err, ctx);
            break;
        case E64_System:
            logSystemError(err, ctx);
            break;
        default:
            logFn(LogLevel_Error, "Unknown error category %u\n", category);
            break;
    }
}
