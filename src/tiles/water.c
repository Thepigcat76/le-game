#include "../../include/shared.h"
#include "../../include/tile.h"
#include <raylib.h>

void water_tile_init() {
  TileType type = {
      .id = TILE_WATER,
      .texture_path = "res/assets/water.png",
      .texture = load_texture("res/assets/water.png"),
      .has_texture = true,
      .is_solid = true,

      .is_ticking = false,
      .stores_custom_data = false,
      .uses_tileset = true,
      .has_animation =true,
  };
  TILES[type.id] = type;
}
