#include "../../include/item.h"
#include "../../include/shared.h"
#include <stdbool.h>

#define SIMPLE_INV_ITEM(item_id, texture_name)                                                                         \
  (ItemType) { .id = item_id, .texture = LoadTexture("res/assets/" texture_name ".png"), .light_source = false }

void grass_item_init() {
  ItemType type = SIMPLE_INV_ITEM(ITEM_GRASS, "grass_inv");
  ITEMS[type.id] = type;
}

void stone_item_init() {
  ItemType type = SIMPLE_INV_ITEM(ITEM_STONE, "stone_inv");
  ITEMS[type.id] = type;
}

void dirt_item_init() {
  ItemType type = SIMPLE_INV_ITEM(ITEM_DIRT, "dirt_inv");
  ITEMS[type.id] = type;
}
