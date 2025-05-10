#include "../../include/shared.h"
#include "../../include/tile.h"
#include <raylib.h>

void stone_tile_init() {
  TileType type = {
      .id = TILE_STONE,
      .texture = load_texture("res/assets/stone.png"),
      .has_texture = true,
      .is_solid = true,

      .is_ticking = false,
      .stores_custom_data = false,
      .uses_tileset = false,
  };
  TILES[type.id] = type;
}
