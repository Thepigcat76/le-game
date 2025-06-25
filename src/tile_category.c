#include "../include/tile/tile_category.h"

char *tile_category_to_string(TileCategory category) {
  switch (category) {
  case TILE_CATEGORY_STONE:
    return "Category Stone";
  case TILE_CATEGORY_WOOD:
    return "Category Wood";
  case TILE_CATEGORY_DIRT:
    return "Category Dirt";
  }
}
