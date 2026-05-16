#pragma once

#include "../category.h"
#include "../data.h"
#include "../shared.h"
#include <raylib.h>
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
  ITEM_SHOVEL,
  // TILE ITEMS
  ITEM_GRASS,
  ITEM_STONE,
  ITEM_DIRT,

  _amount_item_ids,
} ItemId;

typedef struct {
  int attack_damage;
  int break_speed;
  TileCategories break_categories;
} ItemToolProps;

typedef struct {
  ItemId id;

  char *name;
  char *ident;

  AssetId texture;
  bool has_texture;
  bool light_source;
  ItemToolProps tool_props;
} ItemProperties;

typedef struct {
  ItemId id;
} ItemInstance;

extern ItemInstance ITEM_INST_EMPTY;

void item_init(ItemInstance *inst, ItemId id);
