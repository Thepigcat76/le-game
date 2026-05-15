#include "../include/registry.h"
#include <lilc/alloc.h>
#include <lilc/array.h>
#include <limits.h>

static void items_init(RegistryManager *m) {
  m->items[ITEM_EMPTY] = (ItemType){};
}

static void tiles_init(void) {

}

static void registry_manager_init(RegistryManager *manager);

void registries_load(RegistryManager *manager) {
  if (!manager->initialized) {
    registry_manager_init(manager);
  }

}

void registries_unload(RegistryManager *manager) {

}

static void registry_manager_init(RegistryManager *manager) {
  bump_init(&manager->registry_bump, SHRT_MAX);
  bump_allocator_init(&manager->registry_bump_allocator, &manager->registry_bump);

  manager->items = array_new_capacity(ItemType, 256, &HEAP_ALLOCATOR);
  array_fill(manager->items, _amount_item_ids, (ItemType){0});
  manager->tiles = array_new_capacity(TileType, 256, &HEAP_ALLOCATOR);
  array_fill(manager->tiles, _amount_tile_ids, (TileType){0});
  manager->beings = array_new_capacity(BeingType, 256, &HEAP_ALLOCATOR);
  array_fill(manager->beings, _amount_being_ids, (BeingType){0});
  manager->worlds = array_new_capacity(WorldType, 256, &HEAP_ALLOCATOR);
  array_fill(manager->worlds, _amount_world_ids, (WorldType){0});
}
