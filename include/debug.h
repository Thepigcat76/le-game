#pragma once

#include <stdbool.h>
#include "../include/tile.h"

#define SELECTED_TILE_RENDER_POS vec2i(SCREEN_WIDTH - (3.5 * 16) - 30, (SCREEN_HEIGHT / 2.0f) - (3.5 * 8))

extern Vec2f DEBUG_GO_TO_POSITION;
extern int WORLD_BEING_ID;

typedef enum {
    DEBUG_DISPLAY_NONE,
    DEBUG_DISPLAY_ALL_ITEMS,
    DEBUG_DISPLAY_ALL_TILES,
    DEBUG_DISPLAY_ALL_BEINGS,
} DebugDisplayMode;

typedef struct {
    DebugDisplayMode game_object_display;
    bool collisions_enabled;
    bool hitboxes_shown;
    bool instabreak;
    TileInstance selected_tile_to_place_instance;
} DebugOptions;

void debug_init();

void debug_render_overlay();

void debug_render();

void debug_tick();
