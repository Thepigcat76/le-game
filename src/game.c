#include "lilc/array.h"
#include "../include/game.h"
#include "../include/net/client.h"
#include <dirent.h>
#include <raylib.h>
#include <stdint.h>

#define COMMON_RELOAD(game_ptr, src_file_prefix)                                                                                           \
  extern void src_file_prefix##_on_reload(Game *game);                                                                                     \
  src_file_prefix##_on_reload(game_ptr)

GameSide GAME_SIDE;

void game_registry_setup(void) {
  // Items need to be done before tiles,
  // cuz tiles reference the items
  item_types_init();
  tile_types_init();
  world_types_init();
  space_types_init();
}

void game_categories_setup(Game *game) {
  category_add(&game->tile_categories, CATEGORY_STONE, TILE_STONE);
  category_add(&game->tile_categories, CATEGORY_DIRT, TILE_DIRT);
  category_add(&game->tile_categories, CATEGORY_DIRT, TILE_GRASS);
}

void game_feature_add(Game *game, GameFeature game_feature) {
  if (game->cur_save.feature_store.game_features_amount < game->cur_save.feature_store.game_features_capacity) {
    game->cur_save.feature_store.game_features[game->cur_save.feature_store.game_features_amount++] = game_feature;
  }
}

void game_reload(Game *game) {
  COMMON_RELOAD(game, config);
  COMMON_RELOAD(game, save_names);
}

void game_enter_space(Game *game, SpaceDescriptor desc) {
  Space space;
  space_init(&space, desc, game->cur_save.descriptor.config.seed);
  array_add(game->cur_save.spaces, desc);
  array_add(game->cur_save.loaded_spaces, space);
  Space *new_space = &game->cur_save.loaded_spaces[array_len(game->cur_save.loaded_spaces) - 1];
  CLIENT_GAME.world = &new_space->world;
}
