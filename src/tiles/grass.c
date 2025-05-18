#include "../../include/shared.h"
#include "../../include/tile.h"

void grass_tile_init() {
  TileType type = {
      .id = TILE_GRASS,
      .texture_path = "res/assets/grass_tiles.png",
      .texture = load_texture("res/assets/grass_tiles.png"),
      .has_texture = true,
      .is_solid = true,

      .is_ticking = false,
      .stores_custom_data = false,
      .uses_tileset = true,
  };
  TILES[type.id] = type;
}
