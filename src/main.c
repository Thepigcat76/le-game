#include "../include/config.h"
#include "../include/game.h"
#include "../include/shared.h"
#include "../include/ui.h"
#include "raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

void debug_rect(Rectangle *rect) {
  TraceLog(LOG_DEBUG, "Rect{x=%f, y=%f, w=%f, h=%f}", rect->x, rect->y, rect->width, rect->height);
}

float frame_timer = 0;

#define MAX_ANIMATIONS 2

static void update_animation(const TileType *type, float deltaTime) {
  frame_timer += deltaTime * 1000.0f; // to ms
  float delay = 400;
  if (frame_timer >= delay) {
    frame_timer = 0.0f;
    // TODO: Make the 2 dynamic
    TILE_ANIMATION_FRAMES[type->id] = (TILE_ANIMATION_FRAMES[type->id] + 1) % MAX_ANIMATIONS;
  }
}

int main(void) {
#ifdef SURTUR_DEBUG
  SetTraceLogLevel(LOG_DEBUG);
#endif
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ballz");
  InitAudioDevice();
  SetExitKey(0);
  srand(time(NULL));

  shared_init();

  item_types_init();
  tile_types_init();

  game_reload();

  GAME = (Game){
      .player = player_new(),
      .world = world_new(),
      .cur_menu = MENU_START,
      .paused = false,
      .ui_renderer = (UiRenderer){.cur_x = 0,
                                  .cur_y = 0,
                                  .simulate = false,
                                  .ui_height = -1,
                                  .cur_style = {0},
                                  .context = {.screen_width = SCREEN_WIDTH, .screen_height = SCREEN_HEIGHT}},
      .cur_save = -1,
      .detected_saves = 0,
      .debug_options = {.game_object_display = DEBUG_DISPLAY_NONE,
                        .collisions_enabled = true,
                        .hitboxes_shown = false,
                        .selected_tile_to_place_instance = tile_new(&TILES[TILE_DIRT], SELECTED_TILE_RENDER_POS.x + 35,
                                                                    SELECTED_TILE_RENDER_POS.y - 60)}};

  Game *game = &GAME;

  game_init(game);

  player_set_world(&game->player, &game->world);

  world_prepare_rendering(&game->world);

  SetTargetFPS(60);

  int prevWidth = GetScreenWidth();
  int prevHeight = GetScreenHeight();

  Shader shader = LoadShader(NULL, "res/shaders/lighting.fs");
  int light_pos_loc = GetShaderLocation(shader, "lightPos");
  int light_color_loc = GetShaderLocation(shader, "lightColor");
  int light_radius_loc = GetShaderLocation(shader, "lightRadius");
  int ambient_light_loc = GetShaderLocation(shader, "ambientLight");

  Texture2D torch_texture = load_texture("res/assets/torch.png");
  Texture2D slot_texture = load_texture("res/assets/slot.png");
  Texture2D cursor_texture = load_texture("res/assets/cursor.png");
  Texture2D cursor_fist_texture = load_texture("res/assets/cursor_fist.png");

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
    }

    game_tick(game);

    if (IsKeyPressed(KEYBINDS.reload_key)) {
      UnloadShader(shader);
      shader = LoadShader(NULL, "res/shaders/lighting.fs");
      game_reload();
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
          game->player.held_item.type.light_source ? 0.08f * cam->zoom * (1.0f + 0.11f * sin(GetTime())) : 0;

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

            if (IsKeyReleased(KEYBINDS.open_backpack_menu_key)) {
              // game_set_menu(game, MENU_BACKPACK);
            }

#ifdef SURTUR_DEBUG
            debug_tick();

            if (IsKeyPressed(KEY_F1)) {
              world_add_being(&game->world, being_npc_new(game->player.box.x, game->player.box.y));
              WORLD_BEING_ID = game->world.beings_amount - 1;
              TraceLog(LOG_DEBUG, "Set id for being: %d", WORLD_BEING_ID);
            }

            if (IsKeyPressed(KEY_F3)) {
              debug_menu = !debug_menu;

              if (debug_menu) {
                game_set_menu(game, MENU_DEBUG);
              } else {
                game_set_menu(game, MENU_NONE);
              }
            }
#endif

            if (false && game->world.initialized) {
              ssize_t index = world_chunk_index_by_pos(&game->world, game->player.chunk_pos);
              Chunk chunk = game->world.chunks[index];
              for (int y = 0; y < 16; y++) {
                for (int x = 0; x < 16; x++) {
                  TileInstance tile = chunk.tiles[y][x][TILE_LAYER_GROUND];
                  if (CheckCollisionRecs(game->player.box, tile.box)) {
                  }
                }
              }
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

          game_render_overlay(game);
        }

        game_render_menu(game);

        if (game->player.held_item.type.id != ITEM_EMPTY) {
          HideCursor();
          float scale = 3;
          DrawTextureEx(cursor_texture, (Vector2){.x = mousePos.x - 2 * scale, .y = mousePos.y - 1 * scale}, 0, scale,
                        WHITE);
        } else {
          ShowCursor();
        }
      }

      EndDrawing();

      for (int i = 0; i < TILE_TYPE_AMOUNT; i++) {
        update_animation(&TILES[i], GetFrameTime());
      }

      TileInstance *tile_under_player = world_ground_tile_at(&game->world, game->player.tile_pos);
      if (tile_under_player->type.id != TILE_EMPTY) {
      }
    }
  }

  // animation_unload(&water_animation);

  UnloadShader(shader);

  game_unload(game);

  UnloadMusicStream(MUSIC);
  CloseAudioDevice();
  CloseWindow();
}