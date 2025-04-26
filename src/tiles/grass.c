#include "../../include/shared.h"
#include "../../include/tile.h"

void grass_tile_init() {
  TileType type = {
      .id = TILE_GRASS,
      .texture = load_texture("assets/grass.png"),
      .has_texture = true,
      .is_solid = true,

      .is_ticking = false,
      .stores_custom_data = false,
  };
  TILES[type.id] = type;
}