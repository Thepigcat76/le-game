#include "../../include/item.h"
#include "../../include/shared.h"
#include <stdbool.h>

#define SIMPLE_INV_ITEM(item_id, texture_name)                                                                         \
  ITEMS[item_id] =                                                                                                     \
      (ItemType){.id = item_id, .texture = LoadTexture("res/assets/" texture_name ".png"), .light_source = false};     \
  ITEMS_AMOUNT++;

void simple_inv_items_item_init() {
  SIMPLE_INV_ITEM(ITEM_GRASS, "grass_inv");
  SIMPLE_INV_ITEM(ITEM_STONE, "stone_inv");
  SIMPLE_INV_ITEM(ITEM_DIRT, "dirt_inv");
}
