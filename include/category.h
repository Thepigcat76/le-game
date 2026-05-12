#pragma once

#include "lilc/hashmap.h"
#include "lilc/numbers.h"

#define CATEGORY_DIRT "dirt"
#define CATEGORY_STONE "stone"
#define CATEGORY_WOOD "wood"

typedef char **TileCategories;

typedef struct {
  u64 *elems;
} CategoryEntry;

typedef struct {
  const char *category_name;
  Hashmap(char *, CategoryEntry) entries;
} Category;

void category_init(Category *category, const char *category_name);

void category_add(Category *category, const char *name, u64 elem_id);

bool category_contains(Category *category, const char *name, u64 elem_id);

u64 *category_elem_ids(Category *category, const char *name);

char **categories_by_elem_id(Category *category, u64 id, Allocator *allocator);

bool is_category_of_elem_id(Category *category, u64 id, const char *name);
