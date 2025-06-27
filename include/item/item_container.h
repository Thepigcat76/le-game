#pragma once

#include "../bump.h"
#include "../item.h"

typedef struct {
  ItemInstance *items;
  size_t slots;
} ItemContainer;

extern Bump ITEM_CONTAINER_BUMP;

void _internal_item_container_init(void);

ItemContainer item_container_new(size_t slots);

void item_container_set_item_in_slot(ItemContainer *container, ItemInstance item, size_t slot);
