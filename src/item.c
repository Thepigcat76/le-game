#include "../include/item.h"
#include "../include/data.h"
#include <raylib.h>
#include <stdio.h>
#include <string.h>

#define INIT_ITEM(src_file_name)                                                                                                           \
  extern void src_file_name##_item_init();                                                                                                 \
  src_file_name##_item_init();

ItemType ITEMS[MAX_ITEM_TYPES];
size_t ITEMS_AMOUNT = 0;

ItemInstance ITEM_INSTANCE_EMPTY;

void item_types_init() {
  INIT_ITEM(empty);
  INIT_ITEM(simple_items);
  INIT_ITEM(simple_inv_items);
  INIT_ITEM(tool_items);

  ITEM_INSTANCE_EMPTY = (ItemInstance){.type = ITEMS[ITEM_EMPTY]};
}

void item_render(const ItemInstance *item, int x, int y) { DrawTextureEx(item->type.texture, (Vector2){.x = x, .y = y}, 0, 3.5, WHITE); }

char *item_type_to_string(const ItemType *type) {
  if (type == NULL) return "ITEM TYPE IS NULL";

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

ItemType *item_from_str(const char *item_iteral) {
  if (item_iteral == NULL)
    return NULL;

  for (int i = 0; i < ITEMS_AMOUNT; i++) {
    if (strcmp(item_iteral, item_type_to_string(&ITEMS[i])))
      return &ITEMS[i];
  }

  return NULL;
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

void item_save(const ItemInstance *item, DataMap *data) { data_map_insert(data, "item", data_int(item->type.id)); }

void item_load(ItemInstance *item, const DataMap *data) {
  ItemId item_id = data_map_get(data, "item").var.data_int;
  item->type = ITEMS[item_id];
}
