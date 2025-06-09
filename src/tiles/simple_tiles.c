#include "../../include/tile.h"
#include <raylib.h>
#include <stdbool.h>

void workstation_tile_init() {
  TileType type = {
      .id = TILE_WORKSTATION,
      .texture_path = "res/assets/workstation.png",
      .texture = adv_texture_load("res/assets/workstation.png"),
      .has_texture = true,
      .is_solid = true,
      .layer = TILE_LAYER_TOP,
      .tile_color = BROWN,
      .tile_width = 16,
      .tile_height = 32,
      .tile_item = NULL,
      .break_time = 64,

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
      .texture = adv_texture_load("res/assets/tall_oven.png"),
      .has_texture = true,
      .is_solid = true,
      .layer = TILE_LAYER_TOP,
      .tile_color = GRAY,
      .tile_width = 16,
      .tile_height = 48,
      .tile_item = NULL,
      .break_time = 64,

      .is_ticking = false,
      .stores_custom_data = false,
      .uses_tileset = false,
      .has_animation = false,
  };
  TILES[type.id] = type;
}

void tree_tile_init() {
  TileType type = {
      .id = TILE_TREE,
      .texture_path = "res/assets/tree_5.png",
      .texture = adv_texture_load("res/assets/tree_5.png"),
      .has_texture = true,
      .is_solid = true,
      .layer = TILE_LAYER_TOP,
      .tile_color = BROWN,
      .tile_width = 48,
      .tile_height = 80,
      .tile_item = NULL,
      .break_time = 256,

      .is_ticking = false,
      .stores_custom_data = false,
      .uses_tileset = false,
      .has_animation = false,
  };
  TILES[type.id] = type;
}