#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LogLevel_Error = 0,
    LogLevel_Warning = 1,
    LogLevel_Info = 2,
    LogLevel_Debug = 3,
} LogLevel;

static inline void logFn(LogLevel level, const char* fmt, ...)
{
    (void)fmt;
    (void)level;
}

#ifdef __cplusplus
}
#endif
