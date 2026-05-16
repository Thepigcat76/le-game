#pragma once

#include "../data.h"
#include "../data/data_ex.h"
#include "../registries/items.h"
#include "../world.h"
#include "../space.h"

void item_load(ItemInstance *item, const DataMap *data, DataContext ctx);

void world_load(World *world, const DataMap *data, DataContext ctx);

void being_load(BeingInstance *being, const DataMap *data, DataContext ctx);

void item_container_load(ItemContainer *item_container, const DataMap *data, DataContext ctx);

void space_load(SaveDescriptor save_desc, SpaceDescriptor space_desc, Space *space, DataContext ctx);
