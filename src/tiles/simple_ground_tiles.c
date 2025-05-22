#include "../../include/tile.h"
#include <raylib.h>

void empty_tile_init() {
  TileType type = {
      .id = TILE_EMPTY,
      .texture_path = NULL,
      .texture = {},
      .has_texture = false,
      .is_solid = false,
      .layer = TILE_LAYER_GROUND,
      .tile_color = {},
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
  TileType type = {
      .id = TILE_DIRT,
      .texture_path = "res/assets/lighter_dirt_tiles.png",
      .texture = load_texture("res/assets/lighter_dirt_tiles.png"),
      .has_texture = true,
      .is_solid = true,
      .layer = TILE_LAYER_GROUND,
      .tile_color = BROWN,
      .tile_height = TILE_SIZE,
      .tile_item = NULL,

      .is_ticking = false,
      .stores_custom_data = false,
      .uses_tileset = true,
      .has_animation = false,
  };
  TILES[type.id] = type;
}

void grass_tile_init() {
  TileType type = {
      .id = TILE_GRASS,
      .texture_path = "res/assets/grass_tiles.png",
      .texture = load_texture("res/assets/grass_tiles.png"),
      .has_texture = true,
      .is_solid = true,
      .layer = TILE_LAYER_GROUND,
      .tile_color = DARKGREEN,
      .tile_height = TILE_SIZE,
      .tile_item = &ITEMS[ITEM_GRASS],

      .is_ticking = false,
      .stores_custom_data = false,
      .uses_tileset = true,
      .has_animation = false,
  };
  TILES[type.id] = type;
}

void stone_tile_init() {
  TraceLog(LOG_INFO, "Tile item: %s", item_type_to_string(&ITEMS[ITEM_STONE]));
  TileType type = {
      .id = TILE_STONE,
      .texture_path = "res/assets/stone.png",
      .texture = load_texture("res/assets/stone.png"),
      .has_texture = true,
      .is_solid = true,
      .layer = TILE_LAYER_GROUND,
      .tile_color = GRAY,
      .tile_height = TILE_SIZE,
      .tile_item = &ITEMS[ITEM_STONE],

      .is_ticking = false,
      .stores_custom_data = false,
      .uses_tileset = false,
      .has_animation = false,
  };
  TILES[type.id] = type;
}

void workstation_tile_init() {
  TileType type = {
      .id = TILE_WORKSTATION,
      .texture_path = "res/assets/workstation.png",
      .texture = load_texture("res/assets/workstation.png"),
      .has_texture = true,
      .is_solid = true,
      .layer = TILE_LAYER_TOP,
      .tile_color = BROWN,
      .tile_height = 32,
      .tile_item = NULL,

      .is_ticking = false,
      .stores_custom_data = false,
      .uses_tileset = false,
      .has_animation = false,
  };
  TILES[type.id] = type;
}

void oven_tile_init() {
  TileType type = {
      .id = TILE_OVEN,
      .texture_path = "res/assets/tall_oven.png",
      .texture = load_texture("res/assets/tall_oven.png"),
      .has_texture = true,
      .is_solid = true,
      .layer = TILE_LAYER_TOP,
      .tile_color = GRAY,
      .tile_height = 48,
      .tile_item = NULL,

      .is_ticking = false,
      .stores_custom_data = false,
      .uses_tileset = false,
      .has_animation = false,
  };
  TILES[type.id] = type;
}

void water_tile_init() {
  TileType type = {
      .id = TILE_WATER,
      .texture_path = "res/assets/water.png",
      .texture = load_texture("res/assets/water.png"),
      .has_texture = true,
      .is_solid = true,
      .layer = TILE_LAYER_GROUND,
      .tile_color = BLUE,
      .tile_height = TILE_SIZE,
      .tile_item = NULL,

      .is_ticking = false,
      .stores_custom_data = false,
      .uses_tileset = true,
      .has_animation = true,
  };
  TILES[type.id] = type;
}
