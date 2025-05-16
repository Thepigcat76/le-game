#include "../../include/item.h"
#include "../../include/shared.h"
#include <stdbool.h>

void hammer_item_init() {
  ItemType type = {
      .id = ITEM_HAMMER,
      .texture = load_texture("res/assets/hammer.png"),
      .light_source = false,
  };
  ITEMS[type.id] = type;
}
