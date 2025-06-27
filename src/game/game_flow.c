#include "../../include/array.h"
#include "../../include/game.h"
#include <raylib.h>
#include <time.h>

static Bump SOUND_BUMP;
BUMP_ALLOCATOR(SOUND_BUMP_ALLOCATOR, &SOUND_BUMP);

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
  exit(0);
}

// Uses null at the end to terminate
static const char *TEXTURE_MANAGER_TEXTURE_PATHS[TEXTURE_MANAGER_MAX_TEXTURES + 1] = {"cursor", "gui/tool_tip",
                                                                                      "breaking_overlay", "slot", NULL};

void game_init(Game *game) {
  game_init_menu(game);

  bump_init(&SOUND_BUMP, malloc(sizeof(Sound) * 1000), sizeof(Sound) * 1000);

  game->sound_manager.sound_buffers[SOUND_PLACE].base_sound = LoadSound("res/sounds/place_sound.wav");
  game->sound_manager.sound_buffers[SOUND_PLACE].sound_buf =
      array_new_capacity(Sound, SOUND_BUFFER_LIMIT, &SOUND_BUMP_ALLOCATOR);

  for (int i = 0; i < SOUND_BUFFER_LIMIT; i++) {
    game->sound_manager.sound_buffers[SOUND_PLACE].sound_buf[i] =
        LoadSoundAlias(game->sound_manager.sound_buffers[SOUND_PLACE].base_sound);
    SetSoundPitch(game->sound_manager.sound_buffers[SOUND_PLACE].sound_buf[i], 0.5);
    SetSoundVolume(game->sound_manager.sound_buffers[SOUND_PLACE].sound_buf[i], 0.25);
  }

  MUSIC = LoadMusicStream("res/music/main_menu_music.ogg");
  SetMusicVolume(MUSIC, 0.15);
  SetMusicPitch(MUSIC, 0.85);
  // PlayMusicStream(MUSIC);

#ifdef SURTUR_DEBUG
  debug_init();
#endif

  for (int i = 0; TEXTURE_MANAGER_TEXTURE_PATHS[i] != NULL; i++) {
    game->texture_manager.textures[i] = LoadTexture(TextFormat("res/assets/%s.png", TEXTURE_MANAGER_TEXTURE_PATHS[i]));
  }
}

void game_deinit(Game *game) {
  tile_variants_free();

  UnloadMusicStream(MUSIC);

  array_free(game->sound_manager.sound_buffers[SOUND_PLACE].sound_buf);

  array_free(ADV_TILES);

  shaders_unload(&game->shader_manager);

  array_free(game->saves);

  free(SOUND_BUMP.buffer);
  free(GLOBAL_BUMP.buffer);

  free(game->world->chunks);
  free(ITEM_CONTAINER_BUMP.buffer);
}
