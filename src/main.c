#include "../include/camera.h"
#include "../include/config.h"
#include "../include/game.h"
#include "../include/shared.h"
#include "../include/ui.h"
#include "raylib.h"
#include "rlgl.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

void debug_rect(Rectangle *rect) {
  TraceLog(LOG_DEBUG, "Rect{x=%f, y=%f, w=%f, h=%f}", rect->x, rect->y, rect->width, rect->height);
}

float frame_timer = 0;

#define MAX_ANIMATIONS 2

static void update_animations(void) {
  for (int i = 0; i < ANIMATED_TEXTURES_LEN; i++) {
    AnimatedTexture *texture = &ANIMATED_TEXTURES[i];
    texture->frame_timer += GetFrameTime() * 1000.0f;
    float delay = texture->texture.var.texture_animated.frame_time;
    if (texture->frame_timer >= delay) {
      int frames = texture->texture.var.texture_animated.frames;
      texture->cur_frame = (texture->cur_frame + 1) % frames;
      texture->frame_timer = 0;
    }
  }
}

int main(void) {
  game_begin();

  shared_init();
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
                .tile_category_lookup = {}};

  tile_categories_init();

  Game *game = &GAME;

  game_reload(game);

  game_init(game);

  world_prepare_rendering(&game->world);

  int prevWidth = GetScreenWidth();
  int prevHeight = GetScreenHeight();

  Shader shader = LoadShader(NULL, "res/shaders/lighting.fs");
  int light_pos_loc = GetShaderLocation(shader, "lightPos");
  int light_color_loc = GetShaderLocation(shader, "lightColor");
  int light_radius_loc = GetShaderLocation(shader, "lightRadius");
  int ambient_light_loc = GetShaderLocation(shader, "ambientLight");

  Texture2D torch_texture = LoadTexture("res/assets/torch.png");
  Texture2D slot_texture = LoadTexture("res/assets/slot.png");
  Texture2D cursor_texture = LoadTexture("res/assets/cursor.png");
  Texture2D cursor_fist_texture = LoadTexture("res/assets/cursor_fist.png");

  MUSIC = LoadMusicStream("res/music/main_menu_music.ogg");
  SetMusicVolume(MUSIC, 0.15);
  SetMusicPitch(MUSIC, 0.85);
  // PlayMusicStream(MUSIC);

  RenderTexture2D world_texture = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

  UiRenderer *ui_renderer = &game->ui_renderer;

  game_set_menu(game, MENU_START);

  bool debug_menu = false;

  while (!WindowShouldClose()) {
    ui_renderer->cur_x = 0;
    ui_renderer->cur_y = 0;

    UpdateMusicStream(MUSIC);

    int width = GetScreenWidth();
    int height = GetScreenHeight();

    if (width != prevWidth || height != prevHeight) {
      prevWidth = width;
      prevHeight = height;

      world_texture = LoadRenderTexture(width, height);

      camera_focus(&game->player.cam);
    }

    if (!game->paused) {
      game_tick(game);
    }

    if (IsKeyReleased(KEYBINDS.open_close_save_menu_key)) {
      if (game->cur_menu == MENU_SAVE) {
        game->cur_menu = MENU_NONE;
        game->paused = false;
      } else if (game->cur_menu == MENU_NONE) {
        game->cur_menu = MENU_SAVE;
        game->paused = true;
      }
    }

#ifdef SURTUR_DEBUG
    debug_tick();
#endif

    if (IsKeyPressed(KEYBINDS.reload_key)) {
      UnloadShader(shader);
      shader = LoadShader(NULL, "res/shaders/lighting.fs");
      game_reload(game);
    }

    Rectangle slot_rect = {.x = SCREEN_WIDTH - (3.5 * 16) - 30,
                           .y = (SCREEN_HEIGHT / 2.0f) - (3.5 * 8),
                           .width = 20 * 3.5,
                           .height = 20 * 3.5};
    bool slot_selected = CheckCollisionPointRec(GetMousePosition(), slot_rect);

    BeginDrawing();
    {
      ClearBackground(DARKGRAY);

      Camera2D *cam = &game->player.cam;
      // CAMERA BEGIN
      Vector2 mousePos = GetMousePosition();

      Vector2 mouse_world_pos = GetScreenToWorld2D(mousePos, *cam);
      Vector2 light_pos = {(mousePos.x / GetScreenWidth()), 1.0 - (mousePos.y / GetScreenHeight())};

      Vector3 light_color = {1.0f, 1.0f, 0.8f}; // warm white
      float light_radius =
          game->player.held_item.type.item_props.light_source ? 0.08f * cam->zoom * (1.0f + 0.11f * sin(GetTime())) : 0;

      SetShaderValue(shader, light_pos_loc, &light_pos, SHADER_UNIFORM_VEC2);
      SetShaderValue(shader, light_color_loc, &light_color, SHADER_UNIFORM_VEC3);
      SetShaderValue(shader, light_radius_loc, &light_radius, SHADER_UNIFORM_FLOAT);
      SetShaderValue(shader, ambient_light_loc, &CONFIG.ambient_light, SHADER_UNIFORM_FLOAT);
      BeginTextureMode(world_texture);
      {
        BeginMode2D(*cam);
        {
          ClearBackground(DARKGRAY);

          if (!game_cur_menu_hides_game(game)) {
            game_render(game);

            // TODO: MOVE TO GAME RENDER FUNCTION

            game_render_particles(game, false);

            if (IsKeyReleased(KEYBINDS.open_close_backpack_menu_key)) {
              // game_set_menu(game, MENU_BACKPACK);
            }

            // CAMERA END
          }
          EndMode2D();
        }
        EndTextureMode();

        if (!game_cur_menu_hides_game(game)) {
          // RENDER WORLD
          BeginShaderMode(shader);
          {
            DrawTextureRec(world_texture.texture,
                           (Rectangle){0, 0, (float)world_texture.texture.width, -(float)world_texture.texture.height},
                           (Vector2){0, 0}, WHITE);
          }
          EndShaderMode();
        }

        game_render_menu(game);

        if (!game_cur_menu_hides_game(game)) {
          game_render_overlay(game);
        }

        if (game->player.held_item.type.id != ITEM_EMPTY) {
          HideCursor();
          float scale = 3;
          DrawTextureEx(cursor_texture, (Vector2){.x = mousePos.x, .y = mousePos.y}, 0, scale, WHITE);
        } else {
          ShowCursor();
        }
      }
    }
    EndDrawing();

    update_animations();
  }

  // animation_unload(&water_animation);

  UnloadShader(shader);

  shaders_unload(&GAME.shader_manager);

  game_save_cur_save(game);

  game_unload(game);

  game_end();
}