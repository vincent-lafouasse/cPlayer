#include "audio.h"

#include <stdbool.h>
#include <stdlib.h>

static AudioData audiodata_null(void)
{
    return (AudioData){0};
}

AudioData audiodata_new(uint32_t size, uint32_t nChannels, uint32_t sampleRate)
{
    bool hadError = false;

    AudioData track = (AudioData){.data = calloc(nChannels, sizeof(float*)),
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
        audiodata_destroy(&track);
        return audiodata_null();
    }
    return track;
}

void audiodata_destroy(AudioData* track)
{
    if (!track->data) {
        return;
    }
    for (size_t i = 0; i < track->nChannels; ++i) {
        free(track->data[i]);
    }
    free(track->data);
}
