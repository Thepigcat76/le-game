#include "../include/game.h"
#include "../include/config.h"
#include <dirent.h>
#include <raylib.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>

#define COMMON_RELOAD(game_ptr, src_file_prefix)                                                                                           \
  extern void src_file_prefix##_on_reload(Game *game);                                                                                     \
  src_file_prefix##_on_reload(game_ptr)

void game_create(Game *game) {
  GAME = (Game){.debug_options = {.game_object_display = DEBUG_DISPLAY_NONE, .collisions_enabled = true, .hitboxes_shown = false},
                .client_game = NULL,
                .server_game = NULL};
}

Game GAME;

void game_feature_add(Game *game, GameFeature game_feature) {
  if (game->cur_save.feature_store.game_features_amount < game->cur_save.feature_store.game_features_capacity) {
    game->cur_save.feature_store.game_features[game->cur_save.feature_store.game_features_amount++] = game_feature;
  }
}

void game_reload(Game *game) { COMMON_RELOAD(game, config); }
