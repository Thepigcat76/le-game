#include "../../include/item/item_init_helper.h"
#include <stdbool.h>

void empty_item_init() {
  ItemType type = {
      .id = ITEM_EMPTY,
      .texture = {},
      .item_props = {.light_source = false, .tool_props = {.attack_damage = 0, .break_speed = 0}},
  };
  ITEMS[type.id] = type;
  ITEMS_AMOUNT++;
}

void simple_items_item_init() {
  ITEM_TYPE_INIT(ITEM_STICK, "stick", ITEM_PROPS_DEFAULT);
  ITEM_TYPE_INIT(ITEM_TORCH, "torch", {.light_source = true, .tool_props = ITEM_PROPS_DEFAULT.tool_props});
  ITEM_TYPE_INIT(ITEM_MAP, "map", ITEM_PROPS_DEFAULT);
  ITEM_TYPE_INIT(ITEM_BACKPACK, "backpack", ITEM_PROPS_DEFAULT);
}
