#include "../../include/item.h"
#include "../../include/shared.h"
#include <stdbool.h>

void torch_item_init() {
  ItemType type = {
      .id = ITEM_TORCH,
      .texture = load_texture("res/assets/torch.png"),
      .light_source = true,
  };
  ITEMS[type.id] = type;
}
