#include "../include/alloc.h"
#include "../include/camera.h"
#include "../include/config.h"
#include "../include/game.h"
#include "../include/item/item_container.h"
#include "../include/shared.h"
#include "../include/ui.h"
#include "raylib.h"
#include "rlgl.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#define TICK_RATE 20                     // ticks per second
#define TICK_INTERVAL (1.0f / TICK_RATE) // seconds per tick

#define MAX_TICKS_PER_FRAME 20

void debug_rect(Rectangle *rect) {
  TraceLog(LOG_DEBUG, "Rect{x=%f, y=%f, w=%f, h=%f}", rect->x, rect->y, rect->width, rect->height);
}

float frame_timer = 0;

static Texture2D CURSOR_TEXTURE;

#define MAX_ANIMATIONS 2

static void update_animations(void) {
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

int prev_width;
int prev_height;

static void tick(Game *game) {
  game->ui_renderer.cur_x = 0;
  game->ui_renderer.cur_y = 0;

  UpdateMusicStream(MUSIC);

  int width = GetScreenWidth();
  int height = GetScreenHeight();

  if (width != prev_width || height != prev_height) {
    prev_width = width;
    prev_height = height;

    GAME.world_texture = LoadRenderTexture(width, height);

    camera_focus(&game->player.cam);
  }

  if (!game->paused) {
    game_tick(game);
  }

  if (IsKeyReleased(KEYBINDS.open_close_save_menu_key)) {
    if (game->cur_menu == MENU_SAVE) {
      game->cur_menu = MENU_NONE;
      game_set_menu(game, MENU_NONE);
      game->paused = false;
    } else if (game->cur_menu == MENU_NONE) {
      game_set_menu(game, MENU_SAVE);
      game->paused = true;
    }
  }

  if (IsKeyReleased(KEYBINDS.close_cur_menu) && game->cur_menu != MENU_NONE && game->cur_menu != MENU_SAVE) {
    game_set_menu(game, MENU_NONE);
  }

#ifdef SURTUR_DEBUG
  debug_tick();
#endif

  if (IsKeyPressed(KEYBINDS.reload_key)) {
    game_reload(&GAME);
  }

  update_animations();
  game->pressed_keys = (PressedKeys){};
}

static void render(Game *game, float alpha) {
  BeginDrawing();
  {
    ClearBackground(DARKGRAY);

    Camera2D *cam = &GAME.player.cam;
    // CAMERA BEGIN
    Vector2 mousePos = GetMousePosition();

    Vector2 mouse_world_pos = GetScreenToWorld2D(mousePos, *cam);
    Vector2 light_pos = {(mousePos.x / GetScreenWidth()), 1.0 - (mousePos.y / GetScreenHeight())};

    Vector3 light_color = {1.0f, 1.0f, 0.8f}; // warm white
    float light_radius =
        GAME.player.held_item.type.item_props.light_source ? 0.08f * cam->zoom * (1.0f + 0.11f * sin(GetTime())) : 0;

    ShaderVarLookupLighting lighting_lookup = GAME.shader_manager.lookups[SHADER_LIGHTING].var.lighting;
    Shader lighting_shader = GAME.shader_manager.shaders[SHADER_LIGHTING];

    SetShaderValue(lighting_shader, lighting_lookup.light_pos_loc, &light_pos, SHADER_UNIFORM_VEC2);
    SetShaderValue(lighting_shader, lighting_lookup.light_color_loc, &light_color, SHADER_UNIFORM_VEC3);
    SetShaderValue(lighting_shader, lighting_lookup.light_radius_loc, &light_radius, SHADER_UNIFORM_FLOAT);
    SetShaderValue(lighting_shader, lighting_lookup.ambient_light_loc, &CONFIG.ambient_light, SHADER_UNIFORM_FLOAT);
    BeginTextureMode(GAME.world_texture);
    {
      BeginMode2D(*cam);
      {
        ClearBackground(DARKGRAY);

        if (!game_cur_menu_hides_game(&GAME)) {
          game_render(&GAME, alpha);

          // TODO: MOVE TO GAME RENDER FUNCTION

          game_render_particles(&GAME, false);

          if (IsKeyReleased(KEYBINDS.open_close_backpack_menu_key)) {
            // game_set_menu(game, MENU_BACKPACK);
          }

          // CAMERA END
        }
      }
      EndMode2D();
    }
    EndTextureMode();

    if (!game_cur_menu_hides_game(&GAME)) {
      // RENDER WORLD
      BeginShaderMode(lighting_shader);
      {
        DrawTextureRec(
            GAME.world_texture.texture,
            (Rectangle){0, 0, (float)GAME.world_texture.texture.width, -(float)GAME.world_texture.texture.height},
            (Vector2){0, 0}, WHITE);
      }
      EndShaderMode();
    }

    game_render_menu(&GAME);

    if (!game_cur_menu_hides_game(&GAME)) {
      game_render_overlay(&GAME);
    }

    if (GAME.player.held_item.type.id != ITEM_EMPTY) {
      HideCursor();
      float scale = 3;
      DrawTextureEx(CURSOR_TEXTURE, (Vector2){.x = mousePos.x, .y = mousePos.y}, 0, scale, WHITE);
    } else {
      ShowCursor();
    }
  }

  EndDrawing();
}

#define KEY_DOWN(key_name) GAME.pressed_keys.key_name = IsKeyDown(KEYBINDS.key_name)

static void poll_keybinds(Game *game) {
  KEY_DOWN(move_foreward_key);
  KEY_DOWN(move_backward_key);
  KEY_DOWN(move_left_key);
  KEY_DOWN(move_right_key);
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

  CURSOR_TEXTURE = LoadTexture("res/assets/cursor.png");

  MUSIC = LoadMusicStream("res/music/main_menu_music.ogg");
  SetMusicVolume(MUSIC, 0.15);
  SetMusicPitch(MUSIC, 0.85);
  // PlayMusicStream(MUSIC);

  game_set_menu(game, MENU_START);

  while (!WindowShouldClose()) {
    // TODO: Seperate input polling from tick method
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
      ticksPerFrame ++;
    }

    render(game, tickAccumulator / TICK_INTERVAL);
  }

  shaders_unload(&GAME.shader_manager);

  game_save_cur_save(game);

  game_unload(game);

  game_end();
}