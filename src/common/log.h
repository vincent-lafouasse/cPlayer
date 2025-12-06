#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>

#include "Error.h"

#define Ansi_Black "\x1b[0;30m"
#define Ansi_Red "\x1b[0;31m"
#define Ansi_Green "\x1b[0;32m"
#define Ansi_Yellow "\x1b[0;33m"
#define Ansi_Blue "\x1b[0;34m"
#define Ansi_Purple "\x1b[0;35m"
#define Ansi_Cyan "\x1b[0;36m"
#define Ansi_White "\x1b[0;37m"
#define Ansi_Reset "\x1b[0m"

#define LOGGING 1
#define GLOBAL_LOG_LEVEL_THRESHOLD LogLevel_Debug

struct Options;

typedef struct {
    const int argc;
    const char* const* argv;
    const struct Options* options;
} ErrorLogCtx;

void logError(Error err, const ErrorLogCtx* ctx);

typedef enum {
    LogLevel_Error = 0,
    LogLevel_Warning = 1,
    LogLevel_Info = 2,
    LogLevel_Debug = 3,
} LogLevel;

static inline const char* logLevelStr(LogLevel level)
{
    switch (level) {
        case LogLevel_Error:
            return Ansi_Red "[Error  ]" Ansi_Reset;
        case LogLevel_Warning:
            return Ansi_Yellow "[Warning]" Ansi_Reset;
        case LogLevel_Info:
            return Ansi_Green "[Info   ]" Ansi_Reset;
        case LogLevel_Debug:
            return Ansi_Blue "[Debug  ]" Ansi_Reset;
        default:
            return "[?]";
    }
}

static inline void logFn(LogLevel level, const char* fmt, ...)
{
#if LOGGING
    if (level <= GLOBAL_LOG_LEVEL_THRESHOLD) {
        fprintf(stderr, "%s ", logLevelStr(level));

        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
    }
#else
    (void)fmt;
    (void)level;
#endif
}

#ifdef __cplusplus
}
#endif
