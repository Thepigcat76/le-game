#pragma once

#include "../item.h"
#include "../tile/tile_category.h"
#include "raylib.h"

#define ITEM_TYPE_INIT(item_id, texture_path, ...)                                                                     \
  {                                                                                                                    \
    ITEMS[item_id] = (ItemType){.id = item_id,                                                                         \
                                .texture = LoadTexture("res/assets/" texture_path ".png"),                             \
                                .item_props = (ItemProperties)__VA_ARGS__};                                            \
    ITEMS_AMOUNT++;                                                                                                    \
  }

#define TOOL_ITEM_PROPS(...)                                                                                           \
  (ItemProperties) { .light_source = false, .tool_props = (ToolProperties)__VA_ARGS__ }

#define TOOL_PROPS_BREAK_CATEGORIES(...)                                                                               \
  (TileIdCategories) {                                                                                                 \
    .categories = {__VA_ARGS__}, .categories_amount = sizeof((TileCategory[]){__VA_ARGS__}) / sizeof(TileCategory)     \
  }
