#include "../../include/shared.h"
#include "../../include/tile.h"

void empty_tile_init() {
  TileType type = {
      .id = TILE_EMPTY,
      .texture = {},
      .has_texture = false,
      .is_solid = false,

      .is_ticking = false,
      .stores_custom_data = false,
      .uses_tileset = false,
  };
  TILES[type.id] = type;
}
