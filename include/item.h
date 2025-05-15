#pragma once

#include <raylib.h>
#define ITEMS_AMOUNT 2

typedef enum {
  ITEM_EMPTY = 0,
  ITEM_TORCH = 1,
  ITEM_STICK = 2,
} ItemId;

typedef struct {
  ItemId id;
  Texture2D texture;
  bool light_source;
} ItemType;

extern ItemType ITEMS[ITEMS_AMOUNT];

typedef struct {
  ItemType type;
} ItemInstance;

void item_types_init();

void item_type_init(ItemId id);

void item_render(const ItemInstance *item, int x, int y);

char *item_type_to_string(const ItemType *type);
