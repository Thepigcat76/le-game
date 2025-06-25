#include "../include/item/item_container.h"
#include "../include/array.h"

Bump ITEM_CONTAINER_BUMP;
BUMP_ALLOCATOR(ITEM_CONTAINER_BUMP_ALLOCATOR, &ITEM_CONTAINER_BUMP);

void _internal_item_container_init() {
  bump_init(&ITEM_CONTAINER_BUMP, malloc(1024 * sizeof(ItemInstance)), 1024 * sizeof(ItemInstance));
}

ItemContainer item_container_new(size_t slots) {
  ItemContainer item_container = {.items = array_new_capacity(ItemInstance, slots, &ITEM_CONTAINER_BUMP_ALLOCATOR),
                                  .slots = slots};
  for (int i = 0; i < slots; i++) {
    array_add(item_container.items, ITEM_INSTANCE_EMPTY);
  }
  return item_container;
}

void item_container_set_item_in_slot(ItemContainer *container, ItemInstance item, size_t slot) {
  container->items[slot] = item;
}

// TODO: Might want to just write a data list
void item_container_save(const ItemContainer *item_container, DataMap *data) {
  DataList data_list_items = data_list_new(item_container->slots);
  for (int i = 0; i < item_container->slots; i++) {
    DataMap item_data = data_map_new(4);
    item_save(&item_container->items[i], &item_data);
    data_list_add(&data_list_items, data_map(item_data));
  }
  data_map_insert(data, "items", data_list(data_list_items));
}

void item_container_load(ItemContainer *item_container, const DataMap *data) {
  DataList data_list_items = data_map_get(data, "items").var.data_list;
  for (int i = 0; i < item_container->slots; i++) {
    DataMap item_data = data_list_get(&data_list_items, i).var.data_map;
    ItemInstance item;
    item_load(&item, &item_data);
    item_container_set_item_in_slot(item_container, item, i);
  }
}
