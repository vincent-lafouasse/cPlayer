#include "Options.h"

#include <stdlib.h>
#include <string.h>
#include "Error.h"
#include "log.h"

Options defaultOptions(void)
{
    return (Options){.input = NULL, .headless = false};
}

static OptionsResult Ok(Options opt)
{
    return (OptionsResult){.options = opt, .err = NoError};
}

static OptionsResult Err(Error err)
{
    return (OptionsResult){.err = err};
}

static bool strEq(const char* a, const char* b)
{
    return strcmp(a, b) == 0;
}

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

const Flag* matchFlag(const char* s)
{
    for (size_t i = 0; i < nFlags; i++) {
        const char* longFlag = flags[i].longFlag;
        const char* shortFlag = flags[i].shortFlag;

        if (strEq(longFlag, s)) {
            return flags + i;
        }
        if (shortFlag && strEq(shortFlag, s)) {
            return flags + i;
        }
    }

    return NULL;
}

void* bindFlagDestination(const Flag* flag, Options* dest)
{
    if (strEq(flag->id, "headless")) {
        return &dest->headless;
    } else if (strEq(flag->id, "input")) {
        return &dest->input;
    } else {
        return NULL;
    }
}

Error setFlagDestination(const Flag* flag, void* dest, const char* value);

OptionsResult parseOptions(const char** args, size_t sz)
{
    if (sz != 0 && (strEq(args[0], "-h") || strEq(args[0], "--help"))) {
        return Err(E_HelpRequested);
    }

    Options out = defaultOptions();

    size_t i = 0;
    while (i < sz) {
        if (args[i][0] == '-') {
            // flag arguments
            const Flag* flag = matchFlag(args[i]);
            if (flag == NULL) {
                return Err(E_Unknown_Flag);
            }

            void* dest = bindFlagDestination(flag, &out);
            if (flag->type == Flag_Bool) {
                *(bool*)dest = true;
                i++;
            } else if (flag->type == Flag_String) {
                if (i == sz - 1) {
                    return Err(E_Bad_Usage);
                }

                *(const char**)dest = args[i + 1];
                i += 2;
            } else {
                logFn(LogLevel_Error, "Unimplemented flag\n");
            }
        } else {
            // positional arguments
            out.input = args[i++];
        }
    }

    if (out.input == NULL) {
        return Err(E_Bad_Usage);
    } else {
        return Ok(out);
    }
}

const char* boolRepr(bool b)
{
    return b ? "True" : "False";
}

void logOptions(const Options* opts)
{
    logFn(LogLevel_Info, "Options {\n");
    logFn(LogLevel_Info, "\tfile:\t%s\n", opts->input);
    logFn(LogLevel_Info, "\theadless:\t%s\n", boolRepr(opts->headless));
    logFn(LogLevel_Info, "}\n");
}

static size_t flagDisplayWidth(const Flag* f)
{
    if (f->shortFlag) {
        // "-s, --long ARG"
        // len(short) + 2 (comma space) + len(long) + 1 (space) + len(arg)
        return strlen(f->shortFlag) + 2 + strlen(f->longFlag) + 1 +
               strlen(f->argName);
    } else {
        // "--long ARG"
        return strlen(f->longFlag) + 1 + strlen(f->argName);
    }
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
        const size_t pad = maxWidth - flagDisplayWidth(f);

        if (f->shortFlag == NULL) {
            logFn(LogLevel_Info, "\t%s %s%*s %s\n", f->longFlag, f->argName,
                  (int)pad, " ", f->description);
        } else {
            logFn(LogLevel_Info, "\t%s, %s %s%*s %s\n", f->shortFlag,
                  f->longFlag, f->argName, (int)pad, " ", f->description);
        }
    }
}
