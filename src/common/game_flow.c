#include "lilc/array.h"
#include "../../include/game.h"
#include "../../include/reload.h"
#include "../../include/net/client.h"
#include <raylib.h>

/* GAME-INIT-DEINIT */

void game_init(Game *game) {
  common_reload(game);

  category_init(&game->tile_categories, "Tiles");
  category_init(&game->item_categories, "Items");

  game->debug = (Debug){.options = {.game_object_display = DEBUG_DISPLAY_NONE, .collisions_enabled = true, .hitboxes_shown = false}, .game = game};
#ifdef DEBUG_BUILD
  debug_init(&game->debug, game);
#endif
}

void game_deinit(Game *game) {
  tile_variants_free();

  // UnloadMusicStream(MUSIC);

  array_free(ADV_TILES);

  // free(SOUND_BUMP.buffer);
  
  bump_free(&ITEM_CONTAINER_BUMP);
}
