#include "Options.h"

#include <stdlib.h>
#include <string.h>

#include "Error.h"
#include "Flag.h"
#include "log.h"

static OptionsResult Ok(Options opt)
{
    return (OptionsResult){.options = opt, .err = NoError};
}

static OptionsResult Err(Error err, const char* fault)
{
    return (OptionsResult){.err = err, .fault = fault};
}

static bool strEq(const char* a, const char* b)
{
    return strcmp(a, b) == 0;
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

// parseOptions is a deterministic finite state machine
// its alphabet is the different types of tokens: long flag, short flag, values
// etc

// its states
typedef enum { State_Normal, State_ExpectingValue } ParserState;

typedef struct {
    ParserState state;
    const char** tokens;
    size_t i;
    size_t sz;
} Parser;

static Parser parserNew(const char** args, size_t sz);
static bool parserEof(const Parser* parser);
static const char* parserPeek(Parser* parser);
static const char* parserTake(Parser* parser);

OptionsResult parseOptions(const char** args, size_t sz)
{
    Parser parser = parserNew(args, sz);
    Options out = {.input = NULL, .headless = false};

    while (!parserEof(&parser)) {
        const char* token = parserTake(&parser);

        if (token[0] == '-') {
            // flag arguments
            const Flag* flag = matchFlag(token);
            if (flag == NULL) {
                return Err(E_Unknown_Flag, token);
            }
            if (strEq(flag->id, "help")) {
                return Err(E_HelpRequested, NULL);
            }

            void* dest = bindFlagDestination(flag, &out);
            if (flag->type == Flag_Bool) {
                *(bool*)dest = true;
            } else if (flag->type == Flag_String) {
                if (parserEof(&parser)) {
                    return Err(E_Bad_Usage, token);
                }

                *(const char**)dest = parserTake(&parser);
            } else {
                logFn(LogLevel_Error, "Unimplemented flag\n");
                return Err(E_Unimplemented, token);
            }
        } else {
            // positional arguments
            out.input = token;
        }
    }

    if (out.input == NULL) {
        return Err(E_Bad_Usage, NULL);
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
    size_t width;
    if (f->shortFlag) {
        // "-s, --long ARG"
        // len(short) + 2 (comma space) + len(long) [+ 1 (space) + len(arg)]
        width = strlen(f->shortFlag) + 2 + strlen(f->longFlag);
    } else {
        // "--long ARG"
        return strlen(f->longFlag);
    }

    if (f->argName) {
        width += 1 + strlen(f->argName);
    }

    return width;
}

static Parser parserNew(const char** args, size_t sz)
{
    return (Parser){.state = State_Normal, .tokens = args, .i = 0, .sz = sz};
}

static bool parserEof(const Parser* parser)
{
    return parser->i < parser->sz;
}

static const char* parserPeek(Parser* parser)
{
    if (!parserEof(parser)) {
        return parser->tokens[parser->i];
    } else {
        return NULL;
    }
}

static const char* parserTake(Parser* parser)
{
    const char* out = parserPeek(parser);
    parser->i++;
    return out;
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
