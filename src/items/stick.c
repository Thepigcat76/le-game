#include "../../include/item.h"
#include "../../include/shared.h"
#include <stdbool.h>

void stick_item_init() {
  ItemType type = {
      .id = ITEM_STICK,
      .texture = load_texture("res/assets/stick.png"),
      .light_source = false,
  };
  ITEMS[type.id] = type;
}
