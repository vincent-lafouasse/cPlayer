#include "libaudiospecs.h"

#include <stdbool.h>
#include <stdlib.h>

static AudioBuffer audiobuffer_null(void)
{
    return (AudioBuffer){0};
}

AudioBuffer audiobuffer_new(uint32_t size,
                            uint32_t nChannels,
                            uint32_t sampleRate)
{
    bool hadError = false;

    AudioBuffer track = (AudioBuffer){.data = calloc(nChannels, sizeof(float*)),
                                      .size = size,
                                      .nChannels = nChannels,
                                      .sampleRate = sampleRate};
    if (track.data == NULL) {
        hadError = true;
        goto out;
    }

    for (size_t i = 0; i < nChannels; ++i) {
        track.data[i] = calloc(size, sizeof(float));
        if (!track.data[i]) {
            hadError = true;
            goto out;
        }
    }

out:
    if (hadError) {
        audiobuffer_destroy(&track);
        return audiobuffer_null();
    }
    return track;
}

void audiobuffer_destroy(AudioBuffer* track)
{
    if (!track->data) {
        return;
    }
    for (size_t i = 0; i < track->nChannels; ++i) {
        free(track->data[i]);
    }
    free(track->data);
}
