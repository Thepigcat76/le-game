#include "../../include/registry.h"
#include "menus.h"
#include <dlfcn.h>
#include <lilc/alloc.h>
#include <lilc/log.h>

#define register_tile(m, _id, ...)                                                                                                         \
  do {                                                                                                                                     \
    TileProperties t = __VA_ARGS__;                                                                                                        \
    __typeof__(t.id) i = _id;                                                                                                              \
    t.id = i;                                                                                                                              \
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
      bool no_texture;                                                                                                                     \
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
                      .has_texture = !_args.no_texture,                                                                                    \
                  });                                                                                                                      \
  } while (0)

#define register_item(m, _id, ...)                                                                                                         \
  do {                                                                                                                                     \
    ItemProperties t = __VA_ARGS__;                                                                                                        \
    __typeof__(t.id) i = _id;                                                                                                              \
    t.id = i;                                                                                                                              \
    t.has_texture = true;                                                                                                                  \
    array_set(m->items, i, t);                                                                                                             \
  } while (0)

#define register_space(m, _id, ...)                                                                                                        \
  do {                                                                                                                                     \
    SpaceProperties t = __VA_ARGS__;                                                                                                       \
    __typeof__(t.id) i = _id;                                                                                                              \
    t.id = i;                                                                                                                              \
    array_set(m->spaces, i, t);                                                                                                            \
  } while (0)

#define register_menu(m, _id, ...)                                                                                                         \
  do {                                                                                                                                     \
    MenuProperties t = __VA_ARGS__;                                                                                                        \
    __typeof__(t.id) i = _id;                                                                                                              \
    t.id = i;                                                                                                                              \
    array_set(m->menus, i, t);                                                                                                             \
  } while (0)

extern void *dl_handle;

#ifdef RELOADABLE
#define register_menu_renderable(m, _id, _render_func, ...)                                                                                \
  do {                                                                                                                                     \
    MenuProperties t = __VA_ARGS__;                                                                                                        \
    __typeof__(t.id) i = _id;                                                                                                              \
    t.id = i;                                                                                                                              \
    char *_render_func_name = #_render_func;                                                                                               \
    t.render_func = _render_func;                                                                                                          \
    t.render_func = (MenuRenderFunc)dlsym(dl_handle, _render_func_name);                                                                   \
    array_set(m->menus, i, t);                                                                                                             \
  } while (0)
#else
#define register_menu_renderable(m, _id, _render_func, ...)                                                                                \
  do {                                                                                                                                     \
    MenuProperties t = __VA_ARGS__;                                                                                                        \
    __typeof__(t.id) i = _id;                                                                                                              \
    t.id = i;                                                                                                                              \
    t.render_func = _render_func;                                                                                                          \
    array_set(m->menus, i, t);                                                                                                             \
  } while (0)
#endif

static TileCategories shovel_break_categories(void) {
  TileCategories categories = array_new(char *, &HEAP_ALLOCATOR);
  array_add(categories, CATEGORY_DIRT);
  return categories;
}

static void items_init(RegistryManager *m) {
  array_set(m->items, ITEM_EMPTY, (ItemProperties){.id = ITEM_EMPTY});

  register_item(m, ITEM_AXE, {.tool_props = {.attack_damage = 10, .break_speed = 10}, .texture = TEX_IDS[TEX_AXE]});
  register_item(m, ITEM_PICKAXE, {.tool_props = {.attack_damage = 10, .break_speed = 10}, .texture = TEX_IDS[TEX_PICKAXE]});
  register_item(m, ITEM_SHOVEL,
                {.name = "Shovel",
                 .tool_props = {.attack_damage = 10, .break_speed = 10, .break_categories = shovel_break_categories()},
                 .texture = TEX_IDS[TEX_SHOVEL]});
  register_item(m, ITEM_HAMMER, {.tool_props = {.attack_damage = 10, .break_speed = 10}, .texture = TEX_IDS[TEX_HAMMER]});

  register_item(m, ITEM_STICK, {.texture = TEX_IDS[ITEM_STICK]});
  register_item(m, ITEM_TORCH, {.texture = TEX_IDS[TEX_TORCH]});
  register_item(m, ITEM_MAP, {.texture = TEX_IDS[TEX_MAP]});
  register_item(m, ITEM_BACKPACK, {.texture = TEX_IDS[TEX_BACKPACK]});

  register_item(m, ITEM_DIRT, {.texture = TEX_IDS[TEX_DIRT_INV]});
  register_item(m, ITEM_GRASS, {.texture = TEX_IDS[TEX_GRASS_INV]});
  register_item(m, ITEM_STONE, {.texture = TEX_IDS[TEX_STONE_INV]});
}

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
                            .break_time = 2000,
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
                            .texture = TEX_IDS[TEX_LIGHTER_DIRT_TILES],
                            .tile_color = BROWN,
                            .tile_item = ITEM_DIRT,
                            .uses_tileset = true,
                        });
  register_tile_default(m, TILE_WATER,
                        {
                            .layer = TILE_LAYER_GROUND,
                            .tile_color = color_rgba(0, 100, 255, 155),
                            .uses_tileset = true,
                            .no_texture = true,
                            .texture = TEX_IDS[TEX_AXE],
                        });
  register_tile_default(m, TILE_DUNGEON_FLOOR,
                        {
                            .layer = TILE_LAYER_GROUND,
                            .texture = TEX_IDS[TEX_DUNGEON_FLOOR],
                            .tile_color = GRAY,
                            .uses_tileset = true,
                        });
}

static void spaces_init(RegistryManager *m) {
  register_space(m, SPACE_BASE, {.name = "Base", .ident = "base", .default_space = true, .global = true});
  register_space(m, SPACE_DUNGEON_TEST, {.name = "Dungeon Test", .ident = "dungeon"});
}

static void menus_init(RegistryManager *m) {
  register_menu(m, MENU_NONE, {0});
  register_menu_renderable(m, MENU_SAVE, save_menu_render, {.pauses_game = true, .render_func = save_menu_render});
  register_menu_renderable(m, MENU_START, start_menu_render, {.pauses_game = true, .hides_game = true, .render_func = start_menu_render});
  register_menu_renderable(m, MENU_BACKPACK, backpack_menu_render, {.render_func = backpack_menu_render});
  register_menu_renderable(m, MENU_DEBUG, debug_menu_render, {.render_func = debug_menu_render});
  register_menu_renderable(m, MENU_NEW_SAVE, new_save_menu_render,
                           {.pauses_game = true, .hides_game = true, .render_func = new_save_menu_render, .open_func = new_save_menu_open});
  register_menu_renderable(m, MENU_LOAD_SAVE, load_save_menu_render,
                           {.pauses_game = true, .hides_game = true, .render_func = load_save_menu_render});
  register_menu_renderable(m, MENU_MAP, map_menu_render, {.render_func = map_menu_render});
  register_menu_renderable(m, MENU_DIALOG, dialog_menu_render, {.render_func = dialog_menu_render, .open_func = dialog_menu_open});
  register_menu_renderable(m, MENU_INVENTORY, inventory_menu_render, {.container = true, .render_func = inventory_menu_render});
  register_menu_renderable(m, MENU_MULTIPLAYER, multiplayer_menu_render,
                           {.pauses_game = true, .hides_game = true, .render_func = multiplayer_menu_render});
  register_menu_renderable(m, MENU_HOST_SERVER, host_menu_render,
                           {.pauses_game = true, .hides_game = true, .render_func = host_menu_render, .open_func = host_menu_open});
}

void registry_content_load(RegistryManager *manager) {
  items_init(manager);
  tiles_init(manager);
  spaces_init(manager);
  menus_init(manager);
}
