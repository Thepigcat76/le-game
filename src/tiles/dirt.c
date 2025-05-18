#include "../../include/shared.h"
#include "../../include/tile.h"

void dirt_tile_init() {
  TileType type = {
      .id = TILE_DIRT,
      .texture_path = "res/assets/dirt.png",
      .texture = load_texture("res/assets/dirt.png"),
      .has_texture = true,
      .is_solid = true,

      .is_ticking = false,
      .stores_custom_data = false,
      .uses_tileset = false,
  };
  TILES[type.id] = type;
}
