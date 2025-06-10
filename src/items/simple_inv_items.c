#include "../../include/item_init_helper.h"

void simple_inv_items_item_init() {
  ITEM_TYPE_INIT(ITEM_GRASS, "grass_inv", ITEM_PROPS_DEFAULT);
  ITEM_TYPE_INIT(ITEM_STONE, "stone_inv", ITEM_PROPS_DEFAULT);
  ITEM_TYPE_INIT(ITEM_DIRT, "dirt_inv", ITEM_PROPS_DEFAULT);
}
