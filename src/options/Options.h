#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "Error64.h"

typedef struct Options {
    const char* input;
    bool headless;
} Options;

typedef struct {
    Options options;
    Error64 err;
} OptionsResult;

OptionsResult parseOptions(const char** args, size_t sz);
void logOptions(const Options* opts);
void printHelp(const char* programName);
