#include "../../include/net/client.h"
#include "../../include/array.h"
#include "../../include/camera.h"
#include "../../include/game.h"
#include <raylib.h>

#define CLIENT_RELOAD(client_game_ptr, src_file_prefix)                                                                                    \
  extern void src_file_prefix##_on_reload(ClientGame *game);                                                                               \
  src_file_prefix##_on_reload(client_game_ptr);

ClientGame CLIENT_GAME;

static Music MUSIC;

static Bump SOUND_BUMP;
BUMP_ALLOCATOR(SOUND_BUMP_ALLOCATOR, &SOUND_BUMP);

// Uses null at the end to terminate
static const char *TEXTURE_MANAGER_TEXTURE_PATHS[TEXTURE_MANAGER_MAX_TEXTURES + 1] = {"cursor", "gui/tool_tip", "breaking_overlay", "slot",
                                                                                      NULL};

static void client_game_start(void);

static void client_poll_keybinds(ClientGame *client);

static void *client_game(void *args) {
#ifdef DEBUG_BUILD
  SetTraceLogLevel(LOG_DEBUG);
#endif

  // Setup raylib
  // NEEDS TO BE CALLED BEFORE client_init and shared_init, because both load textures
  client_init_raylib();

  // Load Textures, init random...
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
  CLIENT_GAME.world = GAME.world;

  // init registries
  game_registry_init();

  GAME.debug_options.selected_tile_to_place_instance = tile_new(&TILES[TILE_DIRT]);

  GAME.debug_options.selectable_tiles = array_new_capacity(TileInstance, 256, &HEAP_ALLOCATOR);

  tile_categories_init();

  // Reload client resources (initializes them)
  client_reload(&CLIENT_GAME);
  // Reload common resources (initializes them)
  game_reload(&GAME);

  // Setup ticking
  float tick_accumulator = 0.0f;
  float last_frame_time = GetTime();

  int ticks_per_frame = 0;

  // Set initial menu
  client_set_menu(&CLIENT_GAME, MENU_START);

  // Use custom cursor
  HideCursor();

  while (!WindowShouldClose()) {
    client_poll_keybinds(&CLIENT_GAME);
    ticks_per_frame = 0;

    float cur_time = GetTime();
    float delta_time = cur_time - last_frame_time;
    last_frame_time = cur_time;

    tick_accumulator += delta_time;
    while (tick_accumulator >= TICK_INTERVAL && ticks_per_frame < MAX_TICKS_PER_FRAME) {
      game_tick(&GAME);
      if (GAME.world != NULL) {
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

  return NULL;
}

static void *client_packet_listener(void *args) { return NULL; }

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

void client_init(void) {
  CLIENT_GAME = (ClientGame){
      .cur_menu = MENU_START,
      .paused = false,
      .world_texture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight()),
      .local_saves = array_new_capacity(SaveDescriptor, 64, &HEAP_ALLOCATOR),
      .window = {.prev_width = GetScreenWidth(), .prev_height = GetScreenHeight(), .width = GetScreenWidth(), .height = GetScreenHeight()},
      .ui_renderer = (UiRenderer){.cur_x = 0,
                                  .cur_y = 0,
                                  .simulate = false,
                                  .ui_height = -1,
                                  .cur_style = {0},
                                  .initial_style = {0},
                                  .context = {.screen_width = GetScreenWidth(), .screen_height = GetScreenHeight()}}};

  ClientGame *game = &CLIENT_GAME;

  client_init_menu(game);

  bump_init(&SOUND_BUMP, malloc(sizeof(Sound) * 1000), sizeof(Sound) * 1000);

  game->sound_manager.sound_buffers[SOUND_PLACE].base_sound = LoadSound("res/sounds/place_sound.wav");
  game->sound_manager.sound_buffers[SOUND_PLACE].sound_buf = array_new_capacity(Sound, SOUND_BUFFER_LIMIT, &SOUND_BUMP_ALLOCATOR);

  for (int i = 0; i < SOUND_BUFFER_LIMIT; i++) {
    game->sound_manager.sound_buffers[SOUND_PLACE].sound_buf[i] = LoadSoundAlias(game->sound_manager.sound_buffers[SOUND_PLACE].base_sound);
    SetSoundPitch(game->sound_manager.sound_buffers[SOUND_PLACE].sound_buf[i], 0.5);
    SetSoundVolume(game->sound_manager.sound_buffers[SOUND_PLACE].sound_buf[i], 0.25);
  }

  MUSIC = LoadMusicStream("res/music/main_menu_music.ogg");
  SetMusicVolume(MUSIC, 0.15);
  SetMusicPitch(MUSIC, 0.85);
  // PlayMusicStream(MUSIC);

  for (int i = 0; TEXTURE_MANAGER_TEXTURE_PATHS[i] != NULL; i++) {
    game->texture_manager.textures[i] = LoadTexture(TextFormat("res/assets/%s.png", TEXTURE_MANAGER_TEXTURE_PATHS[i]));
  }
}

void client_deinit(ClientGame *client) {
  array_free(client->local_saves);

  array_free(client->sound_manager.sound_buffers[SOUND_PLACE].sound_buf);

  shaders_unload(&client->shader_manager);
}

void client_reload(ClientGame *game) {
  CLIENT_RELOAD(game, tile);
  CLIENT_RELOAD(game, keybinds);
  CLIENT_RELOAD(game, save_names);
  CLIENT_RELOAD(game, shaders);
  CLIENT_RELOAD(game, world);
}

static void client_update_animations(ClientGame *client) {
  for (int i = 0; i < ANIMATED_TEXTURES_LEN; i++) {
    AnimatedTexture *texture = &ANIMATED_TEXTURES[i];
    texture->frame_timer += TICK_INTERVAL * 1000.0f;
    float delay = texture->texture.var.texture_animated.frame_time;
    if (texture->frame_timer >= delay) {
      int frames = texture->texture.var.texture_animated.frames;
      texture->cur_frame = (texture->cur_frame + 1) % frames;
      texture->frame_timer = 0;
    }
  }
}

static bool inv_slot_selected();

void client_tick(ClientGame *client) {
  client->slot_selected = inv_slot_selected();
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

    if (client->player != NULL) {
      camera_focus(&client->player->cam);
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
  if (ui_renderer->ui_height == -1) {
    ui_renderer->cur_y = 0;
    ui_renderer->simulate = true;
    client_render_menu(&CLIENT_GAME);
    ui_renderer->ui_height = ui_renderer->cur_y;

    ui_renderer->simulate = false;
    ui_renderer->cur_x = 0;
    ui_renderer->cur_y = 0;
  }
}

void client_set_menu(ClientGame *game, MenuId menu_id) {
  game->cur_menu = menu_id;
  client_calc_ui_height(&game->ui_renderer);
  client_open_menu(game, menu_id);
}

bool client_cur_menu_hides_game(ClientGame *game) {
  return game->cur_menu == MENU_START || game->cur_menu == MENU_NEW_SAVE || game->cur_menu == MENU_LOAD_SAVE ||
      game->cur_menu == MENU_MULTIPLAYER || game->cur_menu == MENU_HOST_SERVER;
}

static bool inv_slot_selected() {
  Rectangle slot_rect = {
      .x = GetScreenWidth() - (3.5 * 16) - 30, .y = (GetScreenHeight() / 2.0f) - (3.5 * 8), .width = 20 * 3.5, .height = 20 * 3.5};
  return CheckCollisionPointRec(GetMousePosition(), slot_rect);
}

#define KEY_DOWN(key_name)                                                                                                                 \
  client->pressed_keys.key_name##_down |= IsKeyDown(KEYBINDS.key_name);                                                                    \
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
  KEY_DOWN(visit_dungeon_key);
  KEY_DOWN(open_close_inventory_key);
}

void client_join_server(ClientGame *game, const char *ip_addr, uint32_t port) { sockets_connect_to_server(ip_addr, port); }
