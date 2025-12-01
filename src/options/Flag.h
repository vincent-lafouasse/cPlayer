#pragma once

#include <stddef.h>

typedef enum {
    Flag_String,
    Flag_Integer,
    Flag_Bool,
} FlagContainer;

typedef struct {
    const char* id;
    FlagContainer type;
    const char* longFlag;
    const char* shortFlag;
    const char* argName;
    const char* description;
} Flag;

static const Flag flags[] = {
    {.id = "headless",
     .type = Flag_Bool,
     .longFlag = "--headless",
     .shortFlag = NULL,
     .argName = NULL,
     .description = "Decode but do not play"},

    {.id = "input",
     .type = Flag_String,
     .longFlag = "--input",
     .shortFlag = "-i",
     .argName = "file",
     .description = "Audio file to decode"},
};
static const size_t nFlags = sizeof(flags) / sizeof(*flags);

const Flag* matchFlag(const char* s);
