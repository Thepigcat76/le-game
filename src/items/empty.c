#include "../../include/item.h"
#include <stdbool.h>

void empty_item_init() {
  ItemType type = {
      .id = ITEM_EMPTY,
      .texture = {},
  };
  ITEMS[type.id] = type;
}
