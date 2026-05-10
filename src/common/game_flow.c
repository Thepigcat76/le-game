#include "../../include/array.h"
#include "../../include/game.h"
#include "../../include/log.h"
#include "../../include/net/client.h"
#include <raylib.h>

void client_setup_raylib(void) {
#ifdef DEBUG_BUILD
  SetTraceLogLevel(LOG_DEBUG);
#endif
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ballz");
  InitAudioDevice();
  SetExitKey(0);

  SetTargetFPS(60);
}

void client_deinit_raylib(void) {
  CloseAudioDevice();
  CloseWindow();
}
void game_init(Game *game) {
  log_debug("debug stuffs :3");
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
  free(GLOBAL_BUMP.buffer);

  if (game->save_loaded) {
    array_free(CLIENT_WORLD->chunks);
  }
  
  free(ITEM_CONTAINER_BUMP.buffer);
}
