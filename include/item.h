#pragma once

#include "registry.h"
#include "shared.h"
#include "raylib.h"
#include "category.h"
#include "data.h"
#include <stdlib.h>
#include "registries/items.h"
#include "data/data_ex.h"

void item_render(const ItemInstance *item, int x, int y);

void item_tooltip(const ItemInstance *item, char *buf, size_t buf_capacity);

int item_break_speed(const ItemProperties *type);

int item_attack_damage(const ItemProperties *type);

ItemId item_from_str(const char *item_iteral, RegistryManager *registries);

bool item_is_empty(ItemInstance *item);
