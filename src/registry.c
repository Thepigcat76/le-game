#include "../include/registry.h"
#include "../include/net/client.h"
#include <dlfcn.h>
#include <lilc/alloc.h>
#include <lilc/array.h>
#include <lilc/log.h>
#include <limits.h>

TileInstance TILE_INST_EMPTY = {.id = TILE_EMPTY};

ItemInstance ITEM_INST_EMPTY = {.id = ITEM_EMPTY};

static void registry_manager_init(RegistryManager *manager);

typedef void (*RegistryContentLoadFunc)(RegistryManager *manager);

extern void registry_content_load(RegistryManager *manager);

void *dl_handle = NULL;

void registries_load(RegistryManager *manager) {
  if (!manager->initialized) {
    registry_manager_init(manager);
  }

  if (dl_handle != NULL) {
    dlclose(dl_handle);
  }

  dl_handle = dlopen("./build/libfoo.so", RTLD_NOW);
  if (!dl_handle) {
    fprintf(stderr, "dlopen: %s\n", dlerror());
    exit(1);
  }

  dlerror();
  RegistryContentLoadFunc registry_content_load = (RegistryContentLoadFunc)dlsym(dl_handle, "registry_content_load");
  const char *err = dlerror();
  if (err) {
    fprintf(stderr, "dlsym: %s\n", err);
    exit(1);
  }

  registry_content_load(manager);

  log_debug("SPACE IDENT: %s", CLIENT_GAME.game.registries.spaces[0].ident);
}

void registries_unload(RegistryManager *manager) {}

static void registry_manager_init(RegistryManager *manager) {
  bump_init(&manager->registry_bump, SHRT_MAX);
  bump_allocator_init(&manager->registry_bump_allocator, &manager->registry_bump);

  manager->items = array_new_capacity(ItemProperties, _amount_item_ids, &HEAP_ALLOCATOR);
  array_fill(manager->items, _amount_item_ids, (ItemProperties){0});
  manager->tiles = array_new_capacity(TileProperties, _amount_tile_ids, &HEAP_ALLOCATOR);
  array_fill(manager->tiles, _amount_tile_ids, (TileProperties){0});
  manager->beings = array_new_capacity(BeingProperties, _amount_being_ids, &HEAP_ALLOCATOR);
  array_fill(manager->beings, _amount_being_ids, (BeingProperties){0});
  manager->spaces = array_new_capacity(SpaceProperties, _amount_space_ids, &HEAP_ALLOCATOR);
  array_fill(manager->spaces, _amount_space_ids, (SpaceProperties){0});
  manager->menus = array_new_capacity(MenuProperties, _amount_menu_ids, &HEAP_ALLOCATOR);
  array_fill(manager->menus, _amount_space_ids, (MenuProperties){0});
}
