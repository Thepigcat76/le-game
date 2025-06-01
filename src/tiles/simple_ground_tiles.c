#include "../../include/tile.h"
#include <raylib.h>

#define SIMPLE_GROUND_TILE(tile_id, texture_name, tile_color_0, item_type_ptr)                                         \
  (TileType){                                                                                                          \
      .id = tile_id,                                                                                                   \
      .texture_path = "res/assets/" texture_name ".png",                                                               \
      .texture = LoadTexture("res/assets/" texture_name ".png"),                                                      \
      .has_texture = true,                                                                                             \
      .is_solid = true,                                                                                                \
      .layer = TILE_LAYER_GROUND,                                                                                      \
      .tile_color = tile_color_0,                                                                                      \
      .tile_width = TILE_SIZE,                                                                                         \
      .tile_height = TILE_SIZE,                                                                                        \
      .tile_item = item_type_ptr,                                                                                      \
      .is_ticking = false,                                                                                             \
      .stores_custom_data = false,                                                                                     \
      .uses_tileset = false,                                                                                           \
      .has_animation = false,                                                                                          \
  };

#define SIMPLE_TILED_GROUND_TILE(tile_id, texture_name, tile_color_0, item_type_ptr)                                   \
  (TileType){                                                                                                          \
      .id = tile_id,                                                                                                   \
      .texture_path = "res/assets/" texture_name ".png",                                                               \
      .texture = LoadTexture("res/assets/" texture_name ".png"),                                                      \
      .has_texture = true,                                                                                             \
      .is_solid = true,                                                                                                \
      .layer = TILE_LAYER_GROUND,                                                                                      \
      .tile_color = tile_color_0,                                                                                      \
      .tile_width = TILE_SIZE,                                                                                         \
      .tile_height = TILE_SIZE,                                                                                        \
      .tile_item = item_type_ptr,                                                                                      \
      .is_ticking = false,                                                                                             \
      .stores_custom_data = false,                                                                                     \
      .uses_tileset = true,                                                                                            \
      .has_animation = false,                                                                                          \
  };

void empty_tile_init() {
  TileType type = {
      .id = TILE_EMPTY,
      .texture_path = NULL,
      .texture = {},
      .has_texture = false,
      .is_solid = false,
      .layer = TILE_LAYER_GROUND,
      .tile_color = {},
      .tile_width = -1,
      .tile_height = -1,
      .tile_item = NULL,

      .is_ticking = false,
      .stores_custom_data = false,
      .uses_tileset = false,
      .has_animation = false,
  };
  TILES[type.id] = type;
}

void dirt_tile_init() {
  TileType type = SIMPLE_TILED_GROUND_TILE(TILE_DIRT, "lighter_dirt_tiles", BROWN, &ITEMS[ITEM_DIRT]);
  TILES[type.id] = type;
}

void grass_tile_init() {
  TileType type = SIMPLE_TILED_GROUND_TILE(TILE_GRASS, "grass_tiles", DARKGREEN, &ITEMS[ITEM_GRASS]);
  TILES[type.id] = type;
}

void stone_tile_init() {
  TileType type = SIMPLE_TILED_GROUND_TILE(TILE_STONE, "stone", GRAY, &ITEMS[ITEM_STONE]);
  TILES[type.id] = type;
}

void water_tile_init() {
  TileType type = SIMPLE_TILED_GROUND_TILE(TILE_WATER, "water", BLUE, NULL);
  TILES[type.id] = type;
}
