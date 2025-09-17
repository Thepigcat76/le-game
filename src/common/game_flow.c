#include "../../include/array.h"
#include "../../include/game.h"
#include <raylib.h>

void client_init_raylib(void) {
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
#ifdef DEBUG_BUILD
  debug_init();
#endif
}

void game_deinit(Game *game) {
  tile_variants_free();

  // UnloadMusicStream(MUSIC);

  array_free(ADV_TILES);

  // free(SOUND_BUMP.buffer);
  free(GLOBAL_BUMP.buffer);

  if (game->save_loaded) {
    array_free(game->world->chunks);
  }
  
  free(ITEM_CONTAINER_BUMP.buffer);
}
