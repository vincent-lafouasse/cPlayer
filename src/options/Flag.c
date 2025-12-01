#include "Flag.h"

#include <stdbool.h>
#include <string.h>

static bool strEq(const char* a, const char* b)
{
    return strcmp(a, b) == 0;
}

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
