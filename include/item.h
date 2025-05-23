#pragma once

#include "shared.h"
#include <raylib.h>

typedef enum {
  ITEM_EMPTY,
  ITEM_TORCH,
  ITEM_STICK,
  ITEM_HAMMER,
  ITEM_BACKPACK,
  ITEM_MAP,
  // TILE ITEMS
  ITEM_GRASS,
  ITEM_STONE,
  ITEM_DIRT,
} ItemId;

typedef struct {
  ItemId id;
  Texture2D texture;
  bool light_source;
} ItemType;

extern ItemType ITEMS[ITEM_TYPE_AMOUNT];

typedef struct {
  ItemType type;
} ItemInstance;

void item_types_init();

void item_render(const ItemInstance *item, int x, int y);

char *item_type_to_string(const ItemType *type);
