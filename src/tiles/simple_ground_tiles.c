#include "../../include/tile.h"
#include "../../include/tile/tile_init_helper.h"
#include <raylib.h>

void empty_tile_init() {
  array_add(TILES,
            (TileType){
                .id = TILE_EMPTY,
                .texture = {},
                .has_texture = false,
                .layer = TILE_LAYER_GROUND,
                .tile_dimensions = dimf(0, 0),
                .tile_item = NULL,
                .tile_props = {.break_time = 0,
                               .tile_color = TILE_PROPS_DEFAULT.tile_color,
                               .disable_collisions = TILE_PROPS_DEFAULT.disable_collisions},

                .texture_props = {.uses_tileset = false, .has_variants = false},
            });
}

void simple_ground_tiles_tile_init() {
  TILE_TYPE_INIT(TILE_DIRT, TILE_LAYER_GROUND, &ITEMS[ITEM_DIRT], "lighter_dirt_tiles", {.tile_color = BROWN, .break_time = 16},
                 {.uses_tileset = true});
  TILE_TYPE_INIT(TILE_GRASS, TILE_LAYER_GROUND, &ITEMS[ITEM_GRASS], "grass_tiles", {.tile_color = BROWN, .break_time = 16},
                 {.uses_tileset = true});
  TILE_TYPE_INIT(TILE_STONE, TILE_LAYER_GROUND, &ITEMS[ITEM_STONE], "stone", TILE_PROPS_DEFAULT_WITH_COLOR(GRAY));
  TILE_TYPE_INIT(TILE_WATER, TILE_LAYER_GROUND, NULL, "water", {.tile_color = BLUE, .break_time = -1, .disable_collisions = false},
                 {.uses_tileset = true});
}
