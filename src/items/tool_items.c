#include "../../include/item_init_helper.h"
#include <raylib.h>

void tool_items_item_init() {
  ITEM_TYPE_INIT(ITEM_HAMMER, "hammer",
                 TOOL_ITEM_PROPS({.attack_damage = 0,
                                  .break_speed = 0,
                                  .break_categories = TOOL_PROPS_BREAK_CATEGORIES(TILE_CATEGORY_STONE)}));
  ITEM_TYPE_INIT(ITEM_PICKAXE, "pickaxe",
                 TOOL_ITEM_PROPS({.attack_damage = 0,
                                  .break_speed = 0,
                                  .break_categories = TOOL_PROPS_BREAK_CATEGORIES(TILE_CATEGORY_STONE)}));
  ITEM_TYPE_INIT(
      ITEM_AXE, "axe",
      TOOL_ITEM_PROPS(
          {.attack_damage = 0, .break_speed = 0, .break_categories = TOOL_PROPS_BREAK_CATEGORIES(TILE_CATEGORY_WOOD)}));
}