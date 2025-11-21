#pragma once

#include <stdint.h>

#include "FileReader.h"
#include "audio.h"

AudioData decodeWav(FileReader* reader);
