#include "../../include/array.h"
#include "../../include/game.h"
#include <raylib.h>
#include <time.h>

void game_setup_raylib(void) {
#ifdef SURTUR_DEBUG
  SetTraceLogLevel(LOG_DEBUG);
#endif
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ballz");
  InitAudioDevice();
  SetExitKey(0);

  SetTargetFPS(60);
}

void game_deinit_raylib(void) {
  CloseAudioDevice();
  CloseWindow();
}

// Uses null at the end to terminate
static const char *TEXTURE_MANAGER_TEXTURE_PATHS[TEXTURE_MANAGER_MAX_TEXTURES + 1] = {"cursor", "gui/tool_tip",
                                                                                      "breaking_overlay", "slot", NULL};

void game_init(Game *game) {

#ifdef SURTUR_DEBUG
  debug_init();
#endif

  for (int i = 0; TEXTURE_MANAGER_TEXTURE_PATHS[i] != NULL; i++) {
    game->texture_manager.textures[i] = LoadTexture(TextFormat("res/assets/%s.png", TEXTURE_MANAGER_TEXTURE_PATHS[i]));
  }
}

void game_deinit(Game *game) {
  tile_variants_free();

  //UnloadMusicStream(MUSIC);

  array_free(game->sound_manager.sound_buffers[SOUND_PLACE].sound_buf);

  array_free(ADV_TILES);

  shaders_unload(&game->shader_manager);

  array_free(game->saves);

  //free(SOUND_BUMP.buffer);
  free(GLOBAL_BUMP.buffer);

  free(game->world->chunks);
  free(ITEM_CONTAINER_BUMP.buffer);
}
