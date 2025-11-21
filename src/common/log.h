#pragma once

#include <stdarg.h>
#include <stdio.h>

#define LOGGING 1

typedef enum {
    Error = 0,
    Warning = 1,
    Info = 2,
    Debug = 3,
} LogLevel;

static inline const char* logLevelStr(LogLevel level)
{
    switch (level) {
        case Error:
            return "Error";
        case Warning:
            return "Warning";
        case Info:
            return "Info";
        case Debug:
            return "Debug";
        default:
            return "?";
    }
}

#define GLOBAL_LOG_LEVEL_THRESHOLD Debug

static inline void logFn(LogLevel level, const char* fmt, ...)
{
#if LOGGING
    if (level <= GLOBAL_LOG_LEVEL_THRESHOLD) {
        fprintf(stderr, "[%s]\t", logLevelStr(level));

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
