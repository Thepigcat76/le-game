#include "../../include/category.h"
#include <lilc/alloc.h>

TileCategories tile_categories_make(char *categories[]) {
  TileCategories tile_categories = array_new(char *, &HEAP_ALLOCATOR);
  
  size_t i = 0;
  while (categories[i] != NULL) {
    array_add(tile_categories, categories[i]);
    i++;
  }
  
  return tile_categories;
}