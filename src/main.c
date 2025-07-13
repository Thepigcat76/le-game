#include "../include/alloc.h"
#include "../include/game.h"
#include "../include/item/item_container.h"
#include "../include/net/client.h"
#include "../include/net/server.h"
#include "../include/res_loader.h"
#include "../include/shared.h"
#include "raylib.h"
#include "rlgl.h"
#include <stdbool.h>

void debug_rect(Rectangle *rect) { TraceLog(LOG_DEBUG, "Rect{x=%f, y=%f, w=%f, h=%f}", rect->x, rect->y, rect->width, rect->height); }

#define MAX_ANIMATIONS 2

#define KEY_DOWN(key_name)                                                                                                                 \
  client->pressed_keys.key_name##_down |= IsKeyDown(KEYBINDS.key_name);                                                                     \
  client->pressed_keys.key_name##_pressed |= IsKeyPressed(KEYBINDS.key_name);

static void client_poll_keybinds(ClientGame *client) {
  KEY_DOWN(move_foreward_key);
  KEY_DOWN(move_backward_key);
  KEY_DOWN(move_left_key);
  KEY_DOWN(move_right_key);
  KEY_DOWN(zoom_in_key);
  KEY_DOWN(zoom_out_key);
  KEY_DOWN(open_close_save_menu_key);
  KEY_DOWN(open_close_backpack_menu_key);
  KEY_DOWN(open_close_debug_menu_key);
  KEY_DOWN(close_cur_menu_key);
  KEY_DOWN(toggle_hitbox_key);
  KEY_DOWN(reload_key);
}

static void registry_init(void) {
  item_types_init();
  tile_types_init();
}

static void client_start(void) {
  // Very primitive setup of allocators
  alloc_init();

  // Setup raylib
  // NEEDS TO BE CALLED BEFORE client_init and shared_init, because both load textures
  client_init_raylib();

  // Load Textures, init random
  shared_init();

  // Init client and setup client game
  client_init();
  // Setup bump allocator for item containers
  _internal_item_container_init();

  // Create and setup common game
  game_create(&GAME);
  GAME.client_game = &CLIENT_GAME;
  game_init(&GAME);
  CLIENT_GAME.game = &GAME;
  CLIENT_GAME.cur_save = &CLIENT_GAME.game->cur_save;

  // init registries
  registry_init();

  // (Client only) init tile categories
  tile_categories_init();

  // Reload client assets
  client_reload(&CLIENT_GAME);
  // Reload common resources
  game_reload(&GAME);

  float tick_accumulator = 0.0f;
  float last_frame_time = GetTime();

  int ticks_per_frame = 0;

  client_set_menu(&CLIENT_GAME, MENU_START);

  while (!WindowShouldClose()) {
    client_poll_keybinds(&CLIENT_GAME);
    ticks_per_frame = 0;

    float cur_time = GetTime();
    float delta_time = cur_time - last_frame_time;
    last_frame_time = cur_time;

    tick_accumulator += delta_time;
    while (tick_accumulator >= TICK_INTERVAL && ticks_per_frame < MAX_TICKS_PER_FRAME) {
      game_tick(&GAME);
      tick_accumulator -= TICK_INTERVAL;
      ticks_per_frame++;
    }

    client_render(&CLIENT_GAME, tick_accumulator / TICK_INTERVAL);
  }

  client_deinit_raylib();
}

// static void game_run() {
//   // Setup allocators
//   alloc_init();
//
//   // Create window
//   game_begin();
//
//   // Setup random, load textures
//   shared_init();
//   // Setup bump allocator for item containers
//   _internal_item_container_init();
//
//   GAME.player = &GAME.cur_save.player;
//   GAME.world = &GAME.cur_save.world;
//
//   Game *game = &GAME;
//
//   game_reload(game);
//
//   game_init(game);
//
//   float tickAccumulator = 0.0f;
//   float lastFrameTime = GetTime();
//
//   int ticksPerFrame = 0;
//
//   game_set_menu(game, MENU_START);
//
//   while (!WindowShouldClose()) {
//     ticksPerFrame = 0;
//
//     poll_keybinds(game);
//
//     float currentTime = GetTime();
//     float deltaTime = currentTime - lastFrameTime;
//     lastFrameTime = currentTime;
//
//     tickAccumulator += deltaTime;
//     while (tickAccumulator >= TICK_INTERVAL && ticksPerFrame < MAX_TICKS_PER_FRAME) {
//       tick(game);
//       tickAccumulator -= TICK_INTERVAL;
//       ticksPerFrame++;
//     }
//
//     render(game, tickAccumulator / TICK_INTERVAL);
//   }
//
//   game_deinit(game);
//
//   game_end();
// }

int main(int argc, char **argv) {
  bool server = argc > 1 && argv[1];

  if (server) {
    if (argc > 4) {
      char *ip_addr = argv[2];
      char *port = argv[3];

      server_start(ip_addr, atoi(port));
    }
  } else {
    client_start();
  }
}