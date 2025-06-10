#pragma once

#include "shared.h"
#include "raylib.h"
#include "item_props.h"
#include "tile_category.h"
#include <stdlib.h>

typedef enum {
  ITEM_EMPTY,
  ITEM_TORCH,
  ITEM_STICK,
  ITEM_HAMMER,
  ITEM_BACKPACK,
  ITEM_MAP,
  ITEM_AXE,
  ITEM_PICKAXE,
  // TILE ITEMS
  ITEM_GRASS,
  ITEM_STONE,
  ITEM_DIRT,
} ItemId;

typedef struct {
  ItemId id;
  Texture2D texture;
  ItemProperties item_props;
} ItemType;

extern ItemType ITEMS[MAX_ITEM_TYPES];
extern size_t ITEMS_AMOUNT;

typedef struct {
  ItemType type;
} ItemInstance;

void item_types_init();

void item_render(const ItemInstance *item, int x, int y);

char *item_type_to_string(const ItemType *type);

void item_tooltip(const ItemInstance *item, char *buf, size_t buf_capacity);

int item_break_speed(const ItemType *type);

int item_attack_damage(const ItemType *type);
