#pragma once

#include <stdbool.h>

typedef enum {
    DISPLAY_NONE,
    DISPLAY_ALL_ITEMS,
    DISPLAY_ALL_TILES,
} DisplayMode;

typedef struct {
    DisplayMode game_object_display;
    bool collisions_enabled;
    bool hitboxes_shown;
} DebugOptions;

void debug_render();
