#include "../include/game.h"
#include "../include/array.h"
#include <dirent.h>
#include <raylib.h>
#include <stdint.h>
#include <sys/stat.h>

Game GAME;

#define COMMON_RELOAD(game_ptr, src_file_prefix)                                                                                           \
  extern void src_file_prefix##_on_reload(Game *game);                                                                                     \
  src_file_prefix##_on_reload(game_ptr)

void game_create(Game *game) {
  Debug debug = {.options = {.game_object_display = DEBUG_DISPLAY_NONE, .collisions_enabled = true, .hitboxes_shown = false}};
  GAME = (Game){.debug = debug, .client_game = NULL, .server_game = NULL};
}

void game_registry_init(void) {
  // Items need to be done before tiles,
  // cuz tiles reference the items
  item_types_init();
  tile_types_init();
  world_types_init();
  space_types_init();
}

void game_feature_add(Game *game, GameFeature game_feature) {
  if (game->cur_save.feature_store.game_features_amount < game->cur_save.feature_store.game_features_capacity) {
    game->cur_save.feature_store.game_features[game->cur_save.feature_store.game_features_amount++] = game_feature;
  }
}

void game_reload(Game *game) { COMMON_RELOAD(game, config); }

void game_enter_space(Game *game, SpaceDescriptor desc) {
  Space space;
  space_create(desc, game->cur_save.descriptor.config.seed, &space);
  array_add(game->cur_save.spaces, desc);
  array_add(game->cur_save.loaded_spaces, space);
  Space *new_space = &game->cur_save.loaded_spaces[array_len(game->cur_save.loaded_spaces) - 1];
  game->cur_save.cur_space = new_space;
  game->world = &game->cur_save.cur_space->world;
}
