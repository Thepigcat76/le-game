#pragma once

#include "../category.h"
#include "../item.h"
#include "raylib.h"

#define ITEM_TYPE_INIT(item_id, name, asset_id, ...)                                                                                       \
  {                                                                                                                                        \
    ITEMS[item_id] = (ItemType){.id = item_id, .texture = asset_id, .has_texture = true, .item_props = (ItemProperties)__VA_ARGS__};                            \
    ITEMS_AMOUNT++;                                                                                                                        \
  }

#define TOOL_ITEM_PROPS(...)                                                                                                               \
  (ItemProperties) { .light_source = false, .tool_props = (ToolProperties)__VA_ARGS__ }

TileCategories tile_categories_make(char *categories[]);

#define TOOL_PROPS_BREAK_CATEGORIES(...) tile_categories_make((char *[]){__VA_ARGS__ __VA_OPT__(,) NULL})
