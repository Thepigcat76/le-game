#include "../include/item.h"
#include <raylib.h>

#define INIT_ITEM(src_file_name)                                                                                       \
  extern void src_file_name##_item_init();                                                                             \
  src_file_name##_item_init();

ItemType ITEMS[ITEM_TYPE_AMOUNT];

void item_types_init() {
  INIT_ITEM(empty);
  INIT_ITEM(torch);
  INIT_ITEM(stick);
  INIT_ITEM(hammer);
  INIT_ITEM(backpack);
  INIT_ITEM(map);
  INIT_ITEM(grass);
  INIT_ITEM(stone);
  INIT_ITEM(dirt);
}

void item_render(const ItemInstance *item, int x, int y) {
  DrawTextureEx(item->type.texture, (Vector2){.x = x, .y = y}, 0, 3.5, WHITE);
}

char *item_type_to_string(const ItemType *type) {
  switch (type->id) {
  case ITEM_EMPTY:
    return "empty";
  case ITEM_GRASS:
    return "grass";
  case ITEM_STONE:
    return "stone";
  default:
    return "NYI Item";
  }
}
