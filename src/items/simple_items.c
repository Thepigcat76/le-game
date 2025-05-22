#include "../../include/item.h"
#include "../../include/shared.h"
#include <stdbool.h>

void empty_item_init() {
  ItemType type = {
      .id = ITEM_EMPTY,
      .texture = {},
  };
  ITEMS[type.id] = type;
}

void stick_item_init() {
  ItemType type = {
      .id = ITEM_STICK,
      .texture = load_texture("res/assets/stick.png"),
      .light_source = false,
  };
  ITEMS[type.id] = type;
}

void hammer_item_init() {
  ItemType type = {
      .id = ITEM_HAMMER,
      .texture = load_texture("res/assets/hammer.png"),
      .light_source = false,
  };
  ITEMS[type.id] = type;
}

void torch_item_init() {
  ItemType type = {
      .id = ITEM_TORCH,
      .texture = load_texture("res/assets/torch.png"),
      .light_source = true,
  };
  ITEMS[type.id] = type;
}
