#include "../include/alloc.h"
#include "../include/config.h"
#include "../include/game.h"
#include "../include/item/item_container.h"
#include "../include/shared.h"
#include "../include/ui.h"
#include "raylib.h"
#include "rlgl.h"
#include <stdbool.h>
#include <stdlib.h>

void debug_rect(Rectangle *rect) {
  TraceLog(LOG_DEBUG, "Rect{x=%f, y=%f, w=%f, h=%f}", rect->x, rect->y, rect->width, rect->height);
}

float frame_timer = 0;

#define MAX_ANIMATIONS 2

int prev_width;
int prev_height;

static void tick(Game *game) {
  game_tick(game);
}

static void render(Game *game, float alpha) {
  game_render(game, alpha);
}

#define KEY_DOWN(key_name) GAME.pressed_keys.key_name = IsKeyDown(KEYBINDS.key_name)

static void poll_keybinds(Game *game) {
  KEY_DOWN(move_foreward_key);
  KEY_DOWN(move_backward_key);
  KEY_DOWN(move_left_key);
  KEY_DOWN(move_right_key);
  KEY_DOWN(zoom_in_key);
  KEY_DOWN(zoom_out_key);
}

int main(void) {
  // Setup allocators
  alloc_init();

  // Create window
  game_begin();

  // Setup random, load textures
  shared_init();
  // Setup bump allocator for item containers
  _internal_item_container_init();
  // init registries
  item_types_init();
  tile_types_init();

  GAME = (Game){.player = player_new(),
                .world = world_new(),
                .cur_menu = MENU_START,
                .paused = false,
                .ui_renderer = (UiRenderer){.cur_x = 0,
                                            .cur_y = 0,
                                            .simulate = false,
                                            .ui_height = -1,
                                            .cur_style = {0},
                                            .initial_style = {0},
                                            .context = {.screen_width = SCREEN_WIDTH, .screen_height = SCREEN_HEIGHT}},
                .cur_save = -1,
                .detected_saves = 0,
                .debug_options = {.game_object_display = DEBUG_DISPLAY_NONE,
                                  .collisions_enabled = true,
                                  .hitboxes_shown = false,
                                  .selected_tile_to_place_instance = tile_new(TILES[TILE_DIRT])},
                .feature_store = {.game_features = malloc(sizeof(GameFeature) * MAX_GAME_FEATURES_AMOUNT),
                                  .game_features_amount = 0,
                                  .game_features_capacity = MAX_GAME_FEATURES_AMOUNT},
                .sound_manager = {.cur_sound = 0, .sound_timer = 0},
                .particle_manager = {},
                .shader_manager = {},
                .tile_category_lookup = {},
                .pressed_keys = {},
                .world_texture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight())};

  tile_categories_init();

  Game *game = &GAME;

  game_reload(game);

  game_init(game);

  world_prepare_rendering(&game->world);

  prev_width = GetScreenWidth();
  prev_height = GetScreenHeight();

  float tickAccumulator = 0.0f;
  float lastFrameTime = GetTime();

  int ticksPerFrame = 0;

  game_set_menu(game, MENU_START);

  while (!WindowShouldClose()) {
    ticksPerFrame = 0;

    poll_keybinds(game);

    float currentTime = GetTime();
    float deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    tickAccumulator += deltaTime;
    while (tickAccumulator >= TICK_INTERVAL && ticksPerFrame < MAX_TICKS_PER_FRAME) {
      game->tick_delta = tickAccumulator / TICK_INTERVAL;
      tick(game);
      tickAccumulator -= TICK_INTERVAL;
      ticksPerFrame++;
    }

    render(game, tickAccumulator / TICK_INTERVAL);
  }

  shaders_unload(&GAME.shader_manager);

  game_save_cur_save(game);

  game_deinit(game);

  game_end();
}