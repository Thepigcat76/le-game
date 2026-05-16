#include "../include/registry.h"
#include <lilc/alloc.h>
#include <lilc/array.h>
#include <limits.h>
#include <dlfcn.h>

#define register_item(m, _id, ...)                                                                                                         \
  do {                                                                                                                                     \
    ItemProperties t = __VA_ARGS__;                                                                                                        \
    __typeof__(t.id) i = _id;                                                                                                              \
    t.id = i;                                                                                                                              \
    t.has_texture = true;                                                                                                                  \
    array_set(m->items, i, t);                                                                                                             \
  } while (0)

ItemInstance ITEM_INST_EMPTY = {.id = ITEM_EMPTY};

static void items_init(RegistryManager *m) {
  array_set(m->items, ITEM_EMPTY, (ItemProperties){.id = ITEM_EMPTY});

  register_item(m, ITEM_AXE, {.tool_props = {.attack_damage = 10, .break_speed = 10}, .texture = TEX_IDS[TEX_AXE]});
  register_item(m, ITEM_PICKAXE, {.tool_props = {.attack_damage = 10, .break_speed = 10}, .texture = TEX_IDS[TEX_PICKAXE]});
  register_item(m, ITEM_SHOVEL, {.tool_props = {.attack_damage = 10, .break_speed = 10}, .texture = TEX_IDS[TEX_SHOVEL]});
  register_item(m, ITEM_HAMMER, {.tool_props = {.attack_damage = 10, .break_speed = 10}, .texture = TEX_IDS[TEX_HAMMER]});

  register_item(m, ITEM_STICK, {.texture = TEX_IDS[ITEM_STICK]});
  register_item(m, ITEM_TORCH, {.texture = TEX_IDS[TEX_TORCH]});
  register_item(m, ITEM_MAP, {.texture = TEX_IDS[TEX_MAP]});
  register_item(m, ITEM_BACKPACK, {.texture = TEX_IDS[TEX_BACKPACK]});
}

#define register_tile(m, _id, ...)                                                                                                         \
  do {                                                                                                                                     \
    TileProperties t = __VA_ARGS__;                                                                                                        \
    __typeof__(t.id) i = _id;                                                                                                              \
    t.id = i;                                                                                                                              \
    t.has_texture = true;                                                                                                                  \
    array_set(m->tiles, i, t);                                                                                                             \
  } while (0)

#define register_tile_default(m, _id, ...)                                                                                                 \
  do {                                                                                                                                     \
    struct tile_prop_args {                                                                                                                \
      TileLayer layer;                                                                                                                     \
      i32 width;                                                                                                                           \
      i32 height;                                                                                                                          \
      i32 tile_item;                                                                                                                       \
      AssetId texture;                                                                                                                     \
      i32 break_time;                                                                                                                      \
      Color tile_color;                                                                                                                    \
      bool uses_tileset;                                                                                                                   \
      bool disable_collisions;                                                                                                             \
    } _args = __VA_ARGS__;                                                                                                                 \
    if (_args.width == 0)                                                                                                                  \
      _args.width = 16;                                                                                                                    \
    if (_args.height == 0)                                                                                                                 \
      _args.height = 16;                                                                                                                   \
    if (_args.break_time == 0)                                                                                                             \
      _args.break_time = 16;                                                                                                               \
    if (_args.tile_item == 0)                                                                                                              \
      _args.tile_item = -1;                                                                                                                \
    register_tile(m, _id,                                                                                                                  \
                  (TileProperties){                                                                                                        \
                      .layer = _args.layer,                                                                                                \
                      .texture = _args.texture,                                                                                            \
                      .tile_dimensions = dimf(_args.width, _args.height),                                                                  \
                      .tile_item = _args.tile_item,                                                                                        \
                      .break_time = _args.break_time,                                                                                      \
                      .tile_color = _args.tile_color,                                                                                      \
                      .disable_collisions = _args.disable_collisions,                                                                      \
                      .uses_tileset = _args.uses_tileset,                                                                                  \
                  });                                                                                                                      \
  } while (0)

TileInstance TILE_INST_EMPTY = {.id = TILE_EMPTY};

static void tiles_init(RegistryManager *m) {
  array_set(m->tiles, TILE_EMPTY,
            (TileProperties){
                .id = TILE_EMPTY,
                .disable_collisions = true,
            });

  register_tile_default(m, TILE_DIRT,
                        {
                            .layer = TILE_LAYER_GROUND,
                            .texture = TEX_IDS[TEX_DIRT],
                            .tile_color = BROWN,
                            .tile_item = ITEM_DIRT,
                        });
  register_tile_default(m, TILE_GRASS,
                        {
                            .layer = TILE_LAYER_GROUND,
                            .texture = TEX_IDS[TEX_GRASS_TILES],
                            .tile_color = GREEN,
                            .tile_item = ITEM_GRASS,
                            .uses_tileset = true,
                        });
  register_tile_default(m, TILE_STONE,
                        {
                            .layer = TILE_LAYER_GROUND,
                            .texture = TEX_IDS[TEX_STONE],
                            .tile_color = GRAY,
                            .tile_item = ITEM_STONE,
                        });
  register_tile_default(m, TILE_DIRT,
                        {
                            .layer = TILE_LAYER_GROUND,
                            .texture = TEX_IDS[TEX_DIRT],
                            .tile_color = BROWN,
                            .tile_item = ITEM_DIRT,
                            .uses_tileset = true,
                        });
  register_tile_default(m, TILE_WATER,
                        {
                            .layer = TILE_LAYER_GROUND,
                            .texture = TEX_IDS[TEX_WATER],
                            .tile_color = BLUE,
                            .uses_tileset = true,
                        });
  register_tile_default(m, TILE_DUNGEON_FLOOR,
                        {
                            .layer = TILE_LAYER_GROUND,
                            .texture = TEX_IDS[TEX_DUNGEON_FLOOR],
                            .tile_color = GRAY,
                        });
}

static void registry_manager_init(RegistryManager *manager);

typedef void (*PrintTestFunc)(void);

void registries_load(RegistryManager *manager) {
  if (!manager->initialized) {
    registry_manager_init(manager);
  }

  items_init(manager);
  tiles_init(manager);

  void *h = dlopen("./build/libfoo.so", RTLD_NOW);
  if (!h) {
    fprintf(stderr, "dlopen: %s\n", dlerror());
    exit(1);
  }

  dlerror();
  PrintTestFunc print_test_func = (PrintTestFunc)dlsym(h, "print_test");
  const char *err = dlerror();
  if (err) {
    fprintf(stderr, "dlsym: %s\n", err);
    exit(1);
  }

  print_test_func();

  dlclose(h);
}

void registries_unload(RegistryManager *manager) {}

static void registry_manager_init(RegistryManager *manager) {
  bump_init(&manager->registry_bump, SHRT_MAX);
  bump_allocator_init(&manager->registry_bump_allocator, &manager->registry_bump);

  manager->items = array_new_capacity(ItemProperties, 256, &HEAP_ALLOCATOR);
  array_fill(manager->items, _amount_item_ids, (ItemProperties){0});
  manager->tiles = array_new_capacity(TileProperties, 256, &HEAP_ALLOCATOR);
  array_fill(manager->tiles, _amount_tile_ids, (TileProperties){0});
  manager->beings = array_new_capacity(BeingProperties, 256, &HEAP_ALLOCATOR);
  array_fill(manager->beings, _amount_being_ids, (BeingProperties){0});
  manager->worlds = array_new_capacity(WorldProperties, 256, &HEAP_ALLOCATOR);
  array_fill(manager->worlds, _amount_world_ids, (WorldProperties){0});
}
