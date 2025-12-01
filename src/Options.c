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
    const char* name;
    FlagContainer type;
    void* destination;
} FlagSpecs;

const FlagSpecs* matchFlag(const FlagSpecs* flags, size_t nFlags, const char* s)
{
    for (size_t i = 0; i < nFlags; i++) {
        if (strEq(flags[i].name, s)) {
            return flags + i;
        }
    }

    return NULL;
}

OptionsResult parseOptions(const char** args, size_t sz)
{
    Options out = defaultOptions();

    const FlagSpecs flags[] = {
        {.name = "--headless", .type = Flag_Bool, .destination = &out.headless},
        {.name = "-h", .type = Flag_Bool, .destination = &out.headless},
        {.name = "--input", .type = Flag_String, .destination = &out.input},
        {.name = "-i", .type = Flag_String, .destination = &out.input},
    };
    const size_t nFlags = sizeof(flags) / sizeof(*flags);

    size_t i = 0;
    while (i < sz) {
        if (args[i][0] == '-') {
            // flag arguments
            const FlagSpecs* flag = matchFlag(flags, nFlags, args[i]);
            if (flag == NULL) {
                return Err(E_Unknown_Flag);
            }

            if (flag->type == Flag_Bool) {
                *(bool*)flag->destination = true;
                i++;
            } else if (flag->type == Flag_String) {
                if (i == sz - 1) {
                    return Err(E_Bad_Usage);
                }
                *(const char**)flag->destination = args[i + 1];
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
