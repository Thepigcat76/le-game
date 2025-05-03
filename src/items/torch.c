#include "../../include/item.h"
#include "../../include/shared.h"
#include <stdbool.h>

void torch_item_init() {
  ItemType type = {
      .id = ITEM_TORCH,
      .texture = load_texture("res/assets/torch.png"),
  };
  ITEMS[type.id] = type;
}
