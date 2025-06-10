#include "../include/item.h"
#include <raylib.h>
#include <stdio.h>

#define INIT_ITEM(src_file_name)                                                                                       \
  extern void src_file_name##_item_init();                                                                             \
  src_file_name##_item_init();

ItemType ITEMS[MAX_ITEM_TYPES];
size_t ITEMS_AMOUNT = 0;

void item_types_init() {
  INIT_ITEM(empty);
  INIT_ITEM(torch);
  INIT_ITEM(stick);
  INIT_ITEM(hammer);
  INIT_ITEM(backpack);
  INIT_ITEM(map);
  INIT_ITEM(simple_inv_items);
  INIT_ITEM(tool_items);
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

void item_tooltip(const ItemInstance *item, char *buf, size_t buf_capacity) {
  switch (item->type.id) {
  case ITEM_TORCH: {
    snprintf(buf, buf_capacity, "Le Torch\nLe Sus");
    break;
  }
  default:
    buf[0] = '\0';
    break;
  }
}
