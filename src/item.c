#include "../include/item.h"
#include <raylib.h>

#define INIT_ITEM(src_file_name)                                               \
  extern void src_file_name##_item_init();                                     \
  src_file_name##_item_init();

ItemType ITEMS[ITEMS_AMOUNT];

void item_types_init() {
  item_type_init(ITEM_EMPTY);
  item_type_init(ITEM_TORCH);
  item_type_init(ITEM_STICK);
}

void item_type_init(ItemId id) {
  switch (id) {
  case ITEM_EMPTY: {
    INIT_ITEM(empty)
    break;
  }
  case ITEM_TORCH: {
    INIT_ITEM(torch)
    break;
  }
  case ITEM_STICK: {
    INIT_ITEM(stick)
    break;
  }
  }
}

void item_render(const ItemInstance *item, int x, int y) {
  DrawTextureEx(item->type.texture, (Vector2){.x = x, .y = y}, 0, 3.5, WHITE);
}
