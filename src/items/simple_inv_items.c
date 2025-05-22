#include "../../include/item.h"
#include "../../include/shared.h"
#include <stdbool.h>

void grass_item_init() {
  ItemType type = {
      .id = ITEM_GRASS,
      .texture = load_texture("res/assets/grass_inv.png"),
      .light_source = false,
  };
  ITEMS[type.id] = type;
}

void stone_item_init() {
  ItemType type = {
      .id = ITEM_STONE,
      .texture = load_texture("res/assets/stone_inv.png"),
      .light_source = false,
  };
  ITEMS[type.id] = type;
}
