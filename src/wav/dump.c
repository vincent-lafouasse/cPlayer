#include "wav_internals.h"

#include <stdio.h>

#include "log.h"

#define DUMP_PREFIX "./build/dump_"
#define DUMP_SUFFIX ".csv"

void dumpIntCsv(const int* data, unsigned sz, const char* path)
{
#if LOGGING
    FILE* dump = fopen(path, "w");

    for (unsigned i = 0; i < sz; ++i) {
        fprintf(dump, "%i,", data[i]);
    }
    fprintf(dump, "\n");

    fclose(dump);
#else
    (void)data;
    (void)sz;
    (void)path;
#endif
}

void dumpFloatCsv(const float* data, unsigned sz, const char* path)
{
#if LOGGING
    FILE* dump = fopen(path, "w");

    for (unsigned i = 0; i < sz; ++i) {
        fprintf(dump, "%f,", data[i]);
    }
    fprintf(dump, "\n");

    fclose(dump);
#else
    (void)data;
    (void)sz;
    (void)path;
#endif
}
