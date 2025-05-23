#include "../../include/item.h"
#include "../../include/shared.h"
#include <stdbool.h>

#define SIMPLE_INV_ITEM(item_id, texture_name)                                                                         \
  (ItemType) { .id = item_id, .texture = load_texture("res/assets/" texture_name ".png"), .light_source = false }

#define SIMPLE_LIGHT_SOURCE_ITEM(item_id, texture_name)                                                                \
  (ItemType) { .id = item_id, .texture = load_texture("res/assets/" texture_name ".png"), .light_source = true }

void empty_item_init() {
  ItemType type = {
      .id = ITEM_EMPTY,
      .texture = {},
  };
  ITEMS[type.id] = type;
}

void stick_item_init() {
  ItemType type = SIMPLE_INV_ITEM(ITEM_STICK, "stick");
  ITEMS[type.id] = type;
}

void hammer_item_init() {
  ItemType type = SIMPLE_INV_ITEM(ITEM_HAMMER, "hammer");
  ITEMS[type.id] = type;
}

void torch_item_init() {
  ItemType type = SIMPLE_LIGHT_SOURCE_ITEM(ITEM_TORCH, "torch");
  ITEMS[type.id] = type;
}

void map_item_init() {
  ItemType type = SIMPLE_INV_ITEM(ITEM_MAP, "map");
  ITEMS[type.id] = type;
}

void backpack_item_init() {
  ItemType type = SIMPLE_INV_ITEM(ITEM_BACKPACK, "backpack");
  ITEMS[type.id] = type;
}
