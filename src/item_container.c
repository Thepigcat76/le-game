#include "../include/item/item_container.h"
#include "../include/array.h"

Bump ITEM_CONTAINER_BUMP;
BUMP_ALLOCATOR(ITEM_CONTAINER_BUMP_ALLOCATOR, &ITEM_CONTAINER_BUMP);

void _internal_item_container_init() {
  bump_init(&ITEM_CONTAINER_BUMP, malloc(1024 * sizeof(ItemInstance)), 1024 * sizeof(ItemInstance));
}

ItemContainer item_container_new(size_t slots) {
  return (ItemContainer){.items = array_new_capacity(ItemInstance, slots, &ITEM_CONTAINER_BUMP_ALLOCATOR), .slots = slots};
}
