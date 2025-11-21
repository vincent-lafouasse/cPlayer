#pragma once

#include <stdarg.h>
#include <stdio.h>

#define LOGGING 1

static void logFn(const char* fmt, ...)
{
#if LOGGING
    va_list args;

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
#else
    (void)fmt;
#endif
}
