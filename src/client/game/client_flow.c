#include "../../../include/net/client.h"
#include "../../../include/reload.h"
#include "../../../include/game_constants.h"
#include "lilc/log.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <sys/socket.h>

/* CLIENT-INIT-DEINIT */

static void client_init_raylib(void) {
#ifdef DEBUG_BUILD
  SetTraceLogLevel(LOG_DEBUG);
#endif
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(SCREEN_INITIAL_WIDTH, SCREEN_INITIAL_HEIGHT, GAME_TITLE);
  InitAudioDevice();
  SetExitKey(0);

  SetTargetFPS(60);
}

static void client_deinit_raylib(void) {
  CloseAudioDevice();
  CloseWindow();
}

void client_init(ClientGame *client) {
  client_init_raylib();

  pthread_mutex_lock(&CLIENT_MUTEX);
  {
    queue_init(&CLIENT_CONNECTION.queue);

    bump_init(&CLIENT_CONNECTION.packet_bump, 4096 * sizeof(Packet));
    bump_allocator_init(&CLIENT_CONNECTION.packet_bump_allocator, &CLIENT_CONNECTION.packet_bump);
  }
  pthread_mutex_unlock(&CLIENT_MUTEX);

  packets_setup();

  client->packet_logger.log_file = fopen("client_packets.txt", "w");

  int window_width = GetScreenWidth();
  int window_height = GetScreenHeight();

  camera_init(&client->cam, SCREEN_WIDTH, SCREEN_HEIGHT);
  client->state.cur_menu = MENU_START;
  client->state.paused = false;
  client->world_texture = LoadRenderTexture(window_width, window_height);
  client->local_saves = array_new_capacity(SaveDescriptor, 64, &HEAP_ALLOCATOR);
  client->window = (Window){.prev_width = window_height, .prev_height = window_height, .width = window_width, .height = window_height};
  ui_renderer_init(&client->ui_renderer);
  client->players = NULL;
  client->asset_manager = (AssetManager){0};
  client->ui_renderer.asset_manager = &client->asset_manager;

  client_init_menu(client);

  client_reload(client);

  // for (int i = 0; i < SOUND_BUFFER_LIMIT; i++) {
  //   client->sound_manager.sound_buffers[SOUND_PLACE].sound_buf[i] =
  //       LoadSoundAlias(client->sound_manager.sound_buffers[SOUND_PLACE].base_sound);
  //   SetSoundPitch(client->sound_manager.sound_buffers[SOUND_PLACE].sound_buf[i], 0.5);
  //   SetSoundVolume(client->sound_manager.sound_buffers[SOUND_PLACE].sound_buf[i], 0.25);
  // }

  // MUSIC = LoadMusicStream("res/music/main_menu_music.ogg");
  // SetMusicVolume(MUSIC, 0.15);
  // SetMusicPitch(MUSIC, 0.85);
  //  PlayMusicStream(MUSIC);

  AssetId texture_axe = TEX_IDS[TEX_AXE];
  const char *path = client->asset_manager.textures[texture_axe].path;
  log_debug("Axe path: %s", path);

  // for (int i = 0; TEXTURE_MANAGER_TEXTURE_PATHS[i] != NULL; i++) {
  //   client->texture_manager.textures[i] = LoadTexture(TextFormat("res/assets/%s.png", TEXTURE_MANAGER_TEXTURE_PATHS[i]));
  // }
}

void client_deinit(ClientGame *client) {
  fclose(client->packet_logger.log_file);

  array_free(client->local_saves);

  assets_unload(&client->asset_manager);

  client_deinit_raylib();
}

/* INTERRUPTS */

void client_stop_running(ClientGame *game) {
  game->running = false;
  i32 server_addr;
  pthread_mutex_lock(&CLIENT_MUTEX);
  {
    CLIENT_CONNECTION.client_running = false;
    server_addr = CLIENT_CONNECTION.server_addr;
  }
  pthread_mutex_unlock(&CLIENT_MUTEX);

  shutdown(server_addr, SHUT_RDWR);
}

/* SAVES */

void client_init_loaded_save(ClientGame *client, Save *save) {
  client->world = &save->loaded_spaces[0].world;
}
