#pragma once

#include <stdbool.h>
#include "../include/tile.h"

typedef enum {
    DISPLAY_NONE,
    DISPLAY_ALL_ITEMS,
    DISPLAY_ALL_TILES,
} DisplayMode;

typedef struct {
    DisplayMode game_object_display;
    bool collisions_enabled;
    bool hitboxes_shown;
    TileInstance selected_tile_to_place_instance;
} DebugOptions;

void debug_render();
