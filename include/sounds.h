#pragma once

#include "raylib.h"
#include "shared.h"

typedef struct {
    Sound sound_buffer[SOUND_BUFFER_LIMIT];
    float sound_timer;
    int cur_sound;
} SoundManager;
