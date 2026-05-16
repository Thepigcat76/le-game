#pragma once

#include "../data.h"
#include "../data/data_ex.h"
#include "../registries/items.h"
#include "../world.h"
#include "../player.h"
#include "../space.h"

void item_save(const ItemInstance *item, DataMap *data, DataContext ctx);

void world_save(const World *world, DataMap *data, DataContext ctx);

void being_save(const BeingInstance *being, DataMap *data, DataContext ctx);

void item_container_save(const ItemContainer *item_container, DataMap *data, DataContext ctx);

void space_save(SaveDescriptor save_desc, const Space *space, DataContext ctx);

void player_save(const Player *player, DataMap *map, DataContext ctx);

void player_load(Player *player, const DataMap *map, DataContext ctx);
