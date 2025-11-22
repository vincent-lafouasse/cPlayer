#include "play.h"

int stereoOutputCallback(const void* input,
                         void* output,
                         unsigned long frameCount,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void* userData)
{
    (void)input;
    (void)timeInfo;
    (void)statusFlags;

    AudioPlayer* player = (AudioPlayer*)userData;

    float* buffer = (float*)output;
    for (unsigned long i = 0; i < frameCount; ++i) {
        if (player->head < player->len) {
            buffer[2 * i] = player->left[player->head];
            buffer[2 * i + 1] = player->right[player->head];
            player->head += 1;
        } else {
            buffer[2 * i] = 0.0f;
            buffer[2 * i + 1] = 0.0f;
        }
    }

    return 0;
}
