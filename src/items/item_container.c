#include "../../include/item/item_container.h"
#include "../../include/data/load.h"
#include "../../include/data/save.h"
#include "lilc/array.h"
#include <lilc/alloc.h>

Bump ITEM_CONTAINER_BUMP = {0};
static Allocator ITEM_CONTAINER_BUMP_ALLOCATOR = {0};

void _internal_item_container_init(void) {
  bump_init(&ITEM_CONTAINER_BUMP, 1024 * sizeof(ItemInstance));
  bump_allocator_init(&ITEM_CONTAINER_BUMP_ALLOCATOR, &ITEM_CONTAINER_BUMP);
}

void item_container_init(ItemContainer *item_container, size_t slots) {
  item_container->items = array_new_capacity(ItemInstance, slots, &ITEM_CONTAINER_BUMP_ALLOCATOR);
  item_container->slots = slots;
  for (size_t i = 0; i < slots; i++) {
    array_add(item_container->items, ITEM_INST_EMPTY);
  }
}

void item_container_set_item_in_slot(ItemContainer *container, ItemInstance item, size_t slot) { container->items[slot] = item; }

void item_container_insert(ItemContainer *container, ItemInstance item) {
  for (size_t i = 0; i < array_len(container->items); i++) {
    if (item_is_empty(&container->items[i])) {
      item_container_set_item_in_slot(container, item, i);
      break;
    }
  }
}

// TODO: Might want to just write a data list
void item_container_save(const ItemContainer *item_container, DataMap *data, DataContext ctx) {
  DataList data_list_items = data_list_new(item_container->slots);
  for (size_t i = 0; i < item_container->slots; i++) {
    DataMap item_data = data_map_new(4);
    item_save(&item_container->items[i], &item_data, ctx);
    data_list_add(&data_list_items, data_map(item_data));
  }
  data_map_insert(data, "items", data_list(data_list_items));
}

void item_container_load(ItemContainer *item_container, const DataMap *data, DataContext ctx) {
  DataList data_list_items = data_map_get(data, "items").var.data_list;
  for (int i = 0; i < item_container->slots; i++) {
    DataMap item_data = data_list_get(&data_list_items, i).var.data_map;
    ItemInstance item;
    item_load(&item, &item_data, ctx);
    item_container_set_item_in_slot(item_container, item, i);
  }
}
