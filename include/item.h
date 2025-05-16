#pragma once

#include <raylib.h>
#define ITEMS_AMOUNT 4

typedef enum {
  ITEM_EMPTY,
  ITEM_TORCH,
  ITEM_STICK,
  ITEM_HAMMER,
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

void item_render(const ItemInstance *item, int x, int y);

char *item_type_to_string(const ItemType *type);
