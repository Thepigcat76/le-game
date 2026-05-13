#include "../../include/net/client.h"
#include "../../include/camera.h"
#include "../../include/game.h"
#include "../../include/net/packet.h"
#include "../../include/net/payloads.h"
#include "../../include/reload.h"
#include "lilc/array.h"
#include "lilc/log.h"
#include <lilc/alloc.h>
#include <pthread.h>
#include <raylib.h>

NetworkConnection CLIENT_CONNECTION = {.connected = false, .server_addr = -1};
pthread_mutex_t CLIENT_MUTEX = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t CLIENT_COND = PTHREAD_COND_INITIALIZER;

ClientGame CLIENT_GAME = {0};

static Music MUSIC;

static Bump SOUND_BUMP;

static void client_game_start(void);

static void client_poll_keybinds(ClientGame *client);

static void *client_game(void *args) {
  CLIENT_GAME.initializing = true;

  // Setup bump allocator for item containers
  _internal_item_container_init();

  // Setup raylib
  // NEEDS TO BE CALLED BEFORE client_init and shared_init, because both load textures
  client_setup_raylib();
  // init random
  shared_setup();
  // Init client game
  client_init(&CLIENT_GAME);
  // Create and init common game
  game_init(&CLIENT_GAME.game);

  CLIENT_GAME.game.client_game = &CLIENT_GAME;

  Game *game = &CLIENT_GAME.game;

  // init registries
  game_registry_setup();

  game_categories_setup(game);

  game->debug.options.selected_tile_to_place_instance = tile_new(&TILES[TILE_DIRT]);
  game->debug.options.selectable_tiles = array_new_capacity(TileInstance, 256, &HEAP_ALLOCATOR);
  for (size_t i = 0; i < TILES_AMOUNT; i++) {
    array_add(game->debug.options.selectable_tiles, tile_new(&TILES[i]));
  }

  // Setup ticking
  float tick_accumulator = 0.0f;
  float last_frame_time = GetTime();

  int ticks_per_frame = 0;

  // Set initial menu
  client_set_menu(&CLIENT_GAME, MENU_START);

  // Use custom cursor
  HideCursor();

  CLIENT_GAME.initializing = false;

  while (!WindowShouldClose()) {
    client_poll_keybinds(&CLIENT_GAME);
    ticks_per_frame = 0;

    float cur_time = GetTime();
    float delta_time = cur_time - last_frame_time;
    last_frame_time = cur_time;

    tick_accumulator += delta_time;
    while (tick_accumulator >= TICK_INTERVAL && ticks_per_frame < MAX_TICKS_PER_FRAME) {
      game_tick(game);

      pthread_mutex_lock(&CLIENT_MUTEX);
      {
        Packet p;
        if (queue_pop(&CLIENT_CONNECTION.queue, &p)) {
          PacketInfo p_info = PACKET_INFOS[p.id];
          p_info.handle_func(&p);
        }
      }
      pthread_mutex_unlock(&CLIENT_MUTEX);
      if (CLIENT_WORLD != NULL) {
        // printf("Placing tile\n");
        // GAME.world->chunks[0].tiles[0][0][TILE_LAYER_GROUND] = tile_new(&TILES[TILE_GRASS]);
        // world_place_tile(GAME.world, vec2i(0, 0), tile_new(&TILES[TILE_GRASS]));
      }
      tick_accumulator -= TICK_INTERVAL;
      ticks_per_frame++;
    }

    client_render(&CLIENT_GAME, tick_accumulator / TICK_INTERVAL);
  }

  client_deinit_raylib();
  exit(0);

  return NULL;
}

static void *client_packet_listener(void *args) {
  addr_t server_addr = -1;

  pthread_mutex_lock(&CLIENT_MUTEX);
  {
    while (!CLIENT_CONNECTION.connected) {
      log_info("Waiting for server connection...");
      pthread_cond_wait(&CLIENT_COND, &CLIENT_MUTEX);
    }

    server_addr = CLIENT_CONNECTION.server_addr;
  }
  pthread_mutex_unlock(&CLIENT_MUTEX);

  // Listen for packets
  while (true) {
    log_debug("Listening for packets");
    Packet packet = {0};
    log_debug("Packet ptr: %p", packet.payload);
    packet_receive(server_addr, &packet);
    log_debug("Packet: %d", packet.id);

    if (packet.id == PACKET_ERROR) {
      perror("Error packet on client");
      exit(1);
    }

    pthread_mutex_lock(&CLIENT_MUTEX);
    {
      log_debug("Adding packet to queue");
      queue_push(&CLIENT_CONNECTION.queue, packet);
    }
    pthread_mutex_unlock(&CLIENT_MUTEX);
  }

  return NULL;
}

void client_start(void) {
  pthread_t game_thread;
  pthread_t packet_listener_thread;

  if (pthread_create(&game_thread, NULL, client_game, NULL)) {
    printf("Failed to create game thread\n");
    return;
  }

  if (pthread_create(&packet_listener_thread, NULL, client_packet_listener, NULL)) {
    printf("Failed to create packet listener thread\n");
    return;
  }

  pthread_join(game_thread, NULL);
  pthread_join(packet_listener_thread, NULL);
}

void client_init(ClientGame *client) {
  pthread_mutex_lock(&CLIENT_MUTEX);
  {
    queue_init(&CLIENT_CONNECTION.queue);

    bump_init(&CLIENT_CONNECTION.packet_bump, 4096 * sizeof(Packet));
    bump_allocator_init(&CLIENT_CONNECTION.packet_bump_allocator, &CLIENT_CONNECTION.packet_bump);
  }
  pthread_mutex_unlock(&CLIENT_MUTEX);

  packets_setup();

  int window_width = GetScreenWidth();
  int window_height = GetScreenHeight();

  client->cam = camera_new(SCREEN_WIDTH, SCREEN_HEIGHT);
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

  bump_init(&SOUND_BUMP, sizeof(Sound) * 1024);

  client->sound_manager.sound_buffers[SOUND_PLACE].base_sound = LoadSound("res/sounds/place_sound.wav");
  client->sound_manager.sound_buffers[SOUND_PLACE].sound_buf =
      array_new_capacity(Sound, SOUND_BUFFER_LIMIT, &HEAP_ALLOCATOR); // SOUND_BUMP_ALLOCATOR);

  client_reload(client);

  // assets_load(&client->asset_manager);

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
  array_free(client->local_saves);

  array_free(client->sound_manager.sound_buffers[SOUND_PLACE].sound_buf);

  // shaders_unload(&client->shader_manager);
}

static void client_update_animations(ClientGame *client) {
  // for (int i = 0; i < ANIMATED_TEXTURES_LEN; i++) {
  //   AnimatedTexture *texture = &ANIMATED_TEXTURES[i];
  //   texture->frame_timer += TICK_INTERVAL * 1000.0f;
  //   float delay = texture->texture.var.texture_animated.frame_time;
  //   if (texture->frame_timer >= delay) {
  //     int frames = texture->texture.var.texture_animated.frames;
  //     texture->cur_frame = (texture->cur_frame + 1) % frames;
  //     texture->frame_timer = 0;
  //   }
  // }
}

static bool inv_slot_selected();

void client_tick(ClientGame *client) {
  client->state.slot_selected = inv_slot_selected();
  UpdateMusicStream(MUSIC);

  client->window.width = GetScreenWidth();
  client->window.height = GetScreenHeight();

  if (client->window.width != client->window.prev_width || client->window.height != client->window.prev_height) {
    client->window.prev_width = client->window.width;
    client->window.prev_height = client->window.height;

    client->ui_renderer.context.screen_width = client->window.width;
    client->ui_renderer.context.screen_height = client->window.height;

    printf("Resizing, prev: [%d, %d], now: [%d, %d]\n", client->window.width, client->window.height, client->window.prev_width,
           client->window.prev_height);
    UnloadRenderTexture(client->world_texture);
    client->world_texture = LoadRenderTexture(client->window.width, client->window.height);

    if (CLIENT_PLAYER != NULL) {
      log_debug("Focusing camera");
      camera_focus(&client->cam);
    }
  }

  client_update_animations(client);
}

// MENUS

#define INIT_MENU(menu_name)                                                                                                               \
  extern void menu_name##_init();                                                                                                          \
  menu_name##_init();

#define OPEN_MENU(ui_renderer, menu_name)                                                                                                  \
  extern void menu_name##_open(UiRenderer *renderer, const ClientGame *game);                                                              \
  menu_name##_open(ui_renderer, game);

void client_init_menu(ClientGame *game) {
  INIT_MENU(save_menu);
  INIT_MENU(dialog_menu);
  // INIT_MENU(start_menu);
  // INIT_MENU(debug_menu);
}

static void client_open_menu(ClientGame *game, MenuId menu_id) {
  switch (menu_id) {
  case MENU_NEW_SAVE: {
    OPEN_MENU(&game->ui_renderer, new_save_menu);
    break;
  }
  case MENU_HOST_SERVER: {
    OPEN_MENU(&game->ui_renderer, host_menu);
    break;
  }
  default: {
    break;
  }
  }
}

static void client_calc_ui_height(UiRenderer *ui_renderer) {
  if (ui_renderer->ui_height == -1 || ui_renderer->ui_width == -1) {
    ui_renderer->cur_x = 0;
    ui_renderer->cur_y = 0;
    ui_renderer->simulate = true;
    client_render_menu(&CLIENT_GAME);
    ui_renderer->ui_width = ui_renderer->cur_x;
    ui_renderer->ui_height = ui_renderer->cur_y;

    ui_renderer->simulate = false;
    ui_renderer->cur_x = 0;
    ui_renderer->cur_y = 0;
  }
}

void client_set_menu(ClientGame *game, MenuId menu_id) {
  game->state.cur_menu = menu_id;
  client_calc_ui_height(&game->ui_renderer);
  client_open_menu(game, menu_id);
}

bool client_menu_hides_game(ClientGame *game, MenuId menu) {
  return menu == MENU_START || menu == MENU_NEW_SAVE || menu == MENU_LOAD_SAVE || menu == MENU_MULTIPLAYER || menu == MENU_HOST_SERVER;
}

bool client_menu_is_container(ClientGame *game, MenuId menu) { return menu == MENU_INVENTORY || menu == MENU_BACKPACK; }

static bool inv_slot_selected() {
  Rectangle slot_rect = {
      .x = GetScreenWidth() - (3.5 * 16) - 30, .y = (GetScreenHeight() / 2.0f) - (3.5 * 8), .width = 20 * 3.5, .height = 20 * 3.5};
  return CheckCollisionPointRec(GetMousePosition(), slot_rect);
}

#define KEY_DOWN(key_name)                                                                                                                 \
  client->state.pressed_keys.key_name##_down |= IsKeyDown(KEYBINDS.key_name);                                                              \
  client->state.pressed_keys.key_name##_pressed |= IsKeyPressed(KEYBINDS.key_name);

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
  KEY_DOWN(visit_dungeon_key);
  KEY_DOWN(open_close_inventory_key);
}

bool cursor_can_interact_with_tile(ClientGame *client, TileInstance *tile) {
  if (tile == NULL || tile->type == TILE_INSTANCE_EMPTY.type)
    return false;

  Player *p = &client->cur_player;
  bool correct_tool_for_tile = item_tool_correct_for_tile(&p->held_item, tile, &client->game.tile_categories);
  return correct_tool_for_tile;
}

bool cursor_can_interact_with_being(ClientGame *game, BeingInstance *being) {
  if (being == NULL)
    return false;
  return false;
}

addr_t client_join_server(ClientGame *game, const char *ip_addr, uint32_t port) {
  if (!game->connected_to_server) {
    addr_t server_addr = sockets_connect_to_server(ip_addr, port);
    if (server_addr != -1) {
      game->server_addr = server_addr;
      game->connected_to_server = true;
      pthread_mutex_lock(&CLIENT_MUTEX);
      {
        CLIENT_CONNECTION.connected = true;
        CLIENT_CONNECTION.server_addr = server_addr;
        pthread_cond_signal(&CLIENT_COND);
      }
      pthread_mutex_unlock(&CLIENT_MUTEX);
      printf("Successfully connected to server %u, at addr: %s, port: %u\n", server_addr, ip_addr, port);
    } else {
      printf("Failed to connect to server at addr: %s, port: %u\n", ip_addr, port);
    }
    return server_addr;
  }
  return -1;
}

void client_leave_server(ClientGame *game) {
  addr_t server_addr;
  pthread_mutex_lock(&CLIENT_MUTEX);
  {
    PayloadClientDisconnect payload = {.player_id = game->player_id};
    packet_send(CLIENT_CONNECTION.server_addr, C2S_CLIENT_DISCONNECT, &payload);
    CLIENT_CONNECTION.connected = false;
    server_addr = CLIENT_CONNECTION.server_addr;
    CLIENT_CONNECTION.server_addr = -1;
  }
  pthread_mutex_unlock(&CLIENT_MUTEX);
  sockets_close(server_addr);
  game->connected_to_server = false;
}
