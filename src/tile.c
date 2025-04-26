#include "../include/tile.h"

extern TileType TILES[];

// TILE TYPE

#define INIT_TILE(src_file_name)                                               \
  extern void src_file_name##_tile_init();                                     \
  src_file_name##_tile_init();

void tile_type_init(TileId id) {
  switch (id) {
  case TILE_GRASS: {
    INIT_TILE(grass)
    break;
  }
  case TILE_STONE: {
    INIT_TILE(stone)
    break;
  }
  }
}

char *tile_type_to_string(TileType *type) {
  switch (type->id) {
  case TILE_GRASS:
    return "grass";
  case TILE_STONE:
    return "stone";
  }
}

// TILE INSTANCE

void tile_right_click(TileInstance *tile) {}

void tile_tick(TileInstance *tile) {}

void tile_load(TileInstance *tile) {}

void tile_save(TileInstance *tile) {}
