#pragma once

#include "raylib.h"
#include "shared.h"

#define SOUND_MANAGER_SOUNDS 1

typedef enum {
    SOUND_PLACE,
} SoundManagerSound;

typedef struct {
    Sound base_sound;
    // Nullable if only a single sound is required
    Sound *sound_buf;
} SoundBuffer;

typedef struct {
    SoundBuffer sound_buffers[SOUND_BUFFER_LIMIT];
    float sound_timer;
    int cur_sound;
} SoundManager;
