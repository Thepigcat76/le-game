#include "../../include/game.h"
#include <time.h>

void game_begin(void) {
#ifdef SURTUR_DEBUG
  SetTraceLogLevel(LOG_DEBUG);
#endif
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ballz");
  InitAudioDevice();
  SetExitKey(0);
  srand(time(NULL));

  SetTargetFPS(60);
}

void game_end(void) {
  CloseAudioDevice();
  CloseWindow();
}

  void game_init(Game * game) {
    game_init_menu(game);

    PLACE_SOUND = LoadSound("res/sounds/place_sound.wav");

    for (int i = 0; i < SOUND_BUFFER_LIMIT; i++) {
      game->sound_manager.sound_buffer[i] = LoadSoundAlias(PLACE_SOUND);
      SetSoundPitch(game->sound_manager.sound_buffer[i], 0.5);
      SetSoundVolume(game->sound_manager.sound_buffer[i], 0.25);
    }

    BREAK_PROGRESS_TEXTURE = LoadTexture("res/assets/breaking_overlay.png");
    TOOLTIP_TEXTURE = LoadTexture("res/assets/gui/tool_tip.png");
    CURSOR_TEXTURE = LoadTexture("res/assets/cursor.png");

    MUSIC = LoadMusicStream("res/music/main_menu_music.ogg");
    SetMusicVolume(MUSIC, 0.15);
    SetMusicPitch(MUSIC, 0.85);
    // PlayMusicStream(MUSIC);

#ifdef SURTUR_DEBUG
    debug_init();
#endif

    for (int i = 0; TEXTURE_MANAGER_TEXTURE_PATHS[i] != NULL; i++) {
      game->texture_manager.textures[i] =
          LoadTexture(TextFormat("res/assets/%s.png", TEXTURE_MANAGER_TEXTURE_PATHS[i]));
    }
  }


void game_unload(Game *game) {
  tile_variants_free();

  free(game->world.chunks);
  free(game->feature_store.game_features);
  free(GLOBAL_BUMP.buffer);
  free(ITEM_CONTAINER_BUMP.buffer);

  UnloadSound(PLACE_SOUND);
  UnloadMusicStream(MUSIC);
  for (int i = 0; i < SOUND_BUFFER_LIMIT; i++) {
    UnloadSoundAlias(game->sound_manager.sound_buffer[i]);
  }
}
