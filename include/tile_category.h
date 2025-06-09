#pragma once

typedef enum {
  TILE_CATEGORY_WOOD,
  TILE_CATEGORY_STONE,
  TILE_CATEGORY_DIRT,
} TileCategory;

char *tile_category_to_string(TileCategory category);
