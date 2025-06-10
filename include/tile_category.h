#pragma once

#include "../include/shared.h"
#include <stdlib.h>

typedef enum {
  TILE_CATEGORY_WOOD,
  TILE_CATEGORY_STONE,
  TILE_CATEGORY_DIRT,
} TileCategory;

typedef struct {
  TileCategory categories[MAX_CATEGORIES_AMOUNT];
  size_t categories_amount;
} TileIdCategories;

char *tile_category_to_string(TileCategory category);
