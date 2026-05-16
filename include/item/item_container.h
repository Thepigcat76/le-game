#pragma once

#include "lilc/bump.h"
#include "../registries/items.h"

typedef struct {
  ItemInstance *items;
  size_t slots;
} ItemContainer;

extern Bump ITEM_CONTAINER_BUMP;

void _internal_item_container_init(void);

void item_container_init(ItemContainer *item_container, size_t slots);

void item_container_set_item_in_slot(ItemContainer *container, ItemInstance item, size_t slot);

void item_container_insert(ItemContainer *container, ItemInstance item);
