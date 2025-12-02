#include "Flag.h"

#include <string.h>

#include "log.h"

static size_t flagDisplayWidth(const Flag* f)
{
    // "-s, --long ARG"
    size_t width = strlen(f->longFlag);

    if (f->shortFlag) {
        width += strlen(f->shortFlag) + 2;  // comma space
    }

    if (f->argName) {
        width += 1 + strlen(f->argName);  // space before arg
    }

    return width;
}

void printHelp(const char* programName)
{
    logFn(LogLevel_Info, "Usage: %s [OPTIONS] <input>\n", programName);
    logFn(LogLevel_Info, "\n");
    logFn(LogLevel_Info, "Options:\n");

    // Compute padding width for alignment
    size_t maxWidth = 0;
    for (size_t i = 0; i < nFlags; i++) {
        const Flag* f = flags + i;
        size_t width = flagDisplayWidth(f);
        if (width > maxWidth) {
            maxWidth = width;
        }
    }

    // Print help lines
    for (size_t i = 0; i < nFlags; i++) {
        const Flag* f = flags + i;
        const size_t pad = maxWidth + 1 - flagDisplayWidth(f);

        const char* maybeSpace = f->argName ? " " : "";
        const char* maybeArg = f->argName ? f->argName : "";

        if (f->shortFlag == NULL) {
            logFn(LogLevel_Info, "    %s%s%s%*s %s\n", f->longFlag, maybeSpace,
                  maybeArg, (int)pad, " ", f->description);
        } else {
            logFn(LogLevel_Info, "    %s, %s%s%s%*s %s\n", f->shortFlag,
                  f->longFlag, maybeSpace, maybeArg, (int)pad, " ",
                  f->description);
        }
    }
}
