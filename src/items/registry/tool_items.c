#include "../../../include/item/item_init_helper.h"
#include "../../../include/textures.h"
#include <raylib.h>

void tool_items_item_init() {
  ITEM_TYPE_INIT(
      ITEM_HAMMER, "hammer", TEX_IDS[TEX_HAMMER],
      TOOL_ITEM_PROPS({.attack_damage = 0, .break_speed = 0, .break_categories = TOOL_PROPS_BREAK_CATEGORIES(TILE_CATEGORY_STONE)}));
  ITEM_TYPE_INIT(
      ITEM_PICKAXE, "pickaxe", TEX_IDS[TEX_PICKAXE],
      TOOL_ITEM_PROPS({.attack_damage = 0, .break_speed = 0, .break_categories = TOOL_PROPS_BREAK_CATEGORIES(TILE_CATEGORY_STONE)}));
  ITEM_TYPE_INIT(
      ITEM_SHOVEL, "shovel", TEX_IDS[TEX_SHOVEL],
      TOOL_ITEM_PROPS({.attack_damage = 0, .break_speed = 0, .break_categories = TOOL_PROPS_BREAK_CATEGORIES(TILE_CATEGORY_DIRT)}));
  ITEM_TYPE_INIT(
      ITEM_AXE, "axe", TEX_IDS[TEX_AXE],
      TOOL_ITEM_PROPS({.attack_damage = 0, .break_speed = 0, .break_categories = TOOL_PROPS_BREAK_CATEGORIES(TILE_CATEGORY_WOOD)}));
}
