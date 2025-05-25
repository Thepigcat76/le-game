#pragma once

#include <stdbool.h>
#include "../include/tile.h"

#define SELECTED_TILE_RENDER_POS vec2i(SCREEN_WIDTH - (3.5 * 16) - 30, (SCREEN_HEIGHT / 2.0f) - (3.5 * 8))

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

void debug_tick();
