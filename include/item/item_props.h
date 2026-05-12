#pragma once

#include "../category.h"

typedef struct {
  int attack_damage;
  int break_speed;
  TileCategories break_categories;
} ToolProperties;

typedef struct {
  bool light_source;
  ToolProperties tool_props;
} ItemProperties;

extern ItemProperties ITEM_PROPS_DEFAULT;
