#pragma once

#include "../category.h"
#include "../data.h"
#include "../item/item_props.h"
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
  ItemId id;
  AssetId texture;
  bool has_texture;
  ItemProperties item_props;
} ItemType;