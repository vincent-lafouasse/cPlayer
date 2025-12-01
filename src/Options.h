#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "Error.h"

typedef struct {
    const char* input;
    bool headless;
} Options;

typedef struct {
    Options options;
    Error err;
} OptionsResult;

OptionsResult parseOptions(const char** args, size_t sz);
