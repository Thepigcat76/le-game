#include "../include/config.h"
#include "../include/game.h"
#include "../include/shared.h"
#include "../include/ui.h"
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define RENDER_MENU(ui_renderer, menu_name)                                    \
  extern void menu_name##_render(UiRenderer *renderer, const Game *game);      \
  menu_name##_render(ui_renderer, game);

#define SOUND_BUFFER_LIMIT 64
#define SOUND_COOLDOWN 0.125f

static Sound sound_buffer[SOUND_BUFFER_LIMIT] = {0};

void rec_draw_outline(const Rectangle *rec, Color color) {
  DrawRectangleLinesEx(*rec, 1, color);
}

void debug_rect(Rectangle *rect) {
  TraceLog(LOG_DEBUG, "Rect{x=%f, y=%f, w=%f, h=%f}", rect->x, rect->y,
           rect->width, rect->height);
}

void menu_render(UiRenderer *ui_renderer, const Game *game) {
  switch (game->cur_menu) {
  case MENU_SAVE: {
    RENDER_MENU(ui_renderer, save_menu);
    break;
  }
  case MENU_START: {
    RENDER_MENU(ui_renderer, start_menu);
    break;
  }
  case MENU_BACKPACK: {
    RENDER_MENU(ui_renderer, backpack_menu);
    break;
  }
  case MENU_NONE: {
    break;
  }
  }
}

float frame_timer = 0;

static void update_animation(const TileType *type, float deltaTime) {
  frame_timer += deltaTime * 1000.0f; // to ms
  float delay = 400;
  if (frame_timer >= delay) {
    frame_timer = 0.0f;
    // TODO: Make the 2 dynamic
    TILE_ANIMATION_FRAMES[type->id] = (TILE_ANIMATION_FRAMES[type->id] + 1) % 2;
  }
}

int main(void) {
#ifdef SURTUR_DEBUG
  SetTraceLogLevel(LOG_DEBUG);
#endif
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ballz");
  InitAudioDevice();
  SetExitKey(0);
  srand(time(NULL));

  shared_init();

  tile_types_init();
  item_types_init();

  game_reload();

  TileId selected_tile_to_place = TILE_DIRT;
  Vec2i selected_tile_to_place_render_pos =
      vec2i(SCREEN_WIDTH - (3.5 * 16) - 30, (SCREEN_HEIGHT / 2.0f) - (3.5 * 8));
  TileInstance selected_tile_to_place_instance = tile_new(
      &TILES[selected_tile_to_place], selected_tile_to_place_render_pos.x + 35,
      selected_tile_to_place_render_pos.y - 60);

  Game game = {
      .player = player_new(),
      .world = world_new(),
      .cur_menu = MENU_NONE,
      .paused = false,
  };

  player_set_world(&game.player, &game.world);

  if (FileExists("save/game.bin")) {
    game_load(&game);
  } else {
    player_set_pos_ex(&game.player, TILE_SIZE * ((float)CHUNK_SIZE / 2),
                      TILE_SIZE * ((float)CHUNK_SIZE / 2), false);
    world_gen(&game.world);
  }

  world_prepare_rendering(&game.world);

  SetTargetFPS(60);

  float speed = 2.0f;

  Shader shader = LoadShader(NULL, "res/shaders/lighting.fs");
  int light_pos_loc = GetShaderLocation(shader, "lightPos");
  int light_color_loc = GetShaderLocation(shader, "lightColor");
  int light_radius_loc = GetShaderLocation(shader, "lightRadius");
  int ambient_light_loc = GetShaderLocation(shader, "ambientLight");

  Texture2D torch_texture = load_texture("res/assets/torch.png");
  Texture2D slot_texture = load_texture("res/assets/slot.png");
  Texture2D cursor_texture = load_texture("res/assets/cursor.png");
  Texture2D cursor_fist_texture = load_texture("res/assets/cursor_fist.png");

  Sound place_sound = LoadSound("res/sounds/place_sound.wav");

  for (int i = 0; i < SOUND_BUFFER_LIMIT; i++) {
    sound_buffer[i] = LoadSoundAlias(place_sound);
    SetSoundPitch(sound_buffer[i], 0.5);
    SetSoundVolume(sound_buffer[i], 0.25);
  }

  RenderTexture2D world_texture =
      LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

  UiRenderer ui_renderer = {.cur_x = 0,
                            .cur_y = 0,
                            .simulate = false,
                            .ui_height = -1,
                            .cur_style = {0},
                            .game = &game,
                            .context = {.screen_width = SCREEN_WIDTH,
                                        .screen_height = SCREEN_HEIGHT}};

  int cur_sound = 0;

  float sound_timer = 0;

  while (!WindowShouldClose()) {
    ui_renderer.cur_x = 0;
    ui_renderer.cur_y = 0;

    game_tick(&game);

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

    if (ui_renderer.ui_height == -1) {
      ui_renderer.cur_y = 0;
      ui_renderer.simulate = true;
      menu_render(&ui_renderer, &game);
      ui_renderer.ui_height = ui_renderer.cur_y;

      ui_renderer.simulate = false;
      ui_renderer.cur_x = 0;
      ui_renderer.cur_y = 0;
    }

    sound_timer += GetFrameTime();

    BeginDrawing();
    {
      ClearBackground(DARKGRAY);

      Camera2D *cam = &game.player.cam;
      // CAMERA BEGIN
      Vector2 mousePos = GetMousePosition();

      Vector2 mouse_world_pos = GetScreenToWorld2D(mousePos, *cam);
      Vector2 light_pos = {(mousePos.x / GetScreenWidth()),
                           1.0 - (mousePos.y / GetScreenHeight())};

      Vector3 light_color = {1.0f, 1.0f, 0.8f}; // warm white
      float light_radius =
          game.player.held_item.type.light_source
              ? 0.08f * cam->zoom * (1.0f + 0.11f * sin(GetTime()))
              : 0;

      SetShaderValue(shader, light_pos_loc, &light_pos, SHADER_UNIFORM_VEC2);
      SetShaderValue(shader, light_color_loc, &light_color,
                     SHADER_UNIFORM_VEC3);
      SetShaderValue(shader, light_radius_loc, &light_radius,
                     SHADER_UNIFORM_FLOAT);
      SetShaderValue(shader, ambient_light_loc, &CONFIG.ambient_light,
                     SHADER_UNIFORM_FLOAT);
      BeginTextureMode(world_texture);
      {
        BeginMode2D(*cam);
        {
          ClearBackground(DARKGRAY);

          if (game.cur_menu != MENU_START) {
            game_render(&game);

            //DrawTextureEx(workstation_texture, (Vector2){.x = 0, .y = 0}, 0, 1,
            //              WHITE);

            // rec_draw_outline(&tile_box, BLUE);

            int x_index = floor_div(mouse_world_pos.x, TILE_SIZE);
            int y_index = floor_div(mouse_world_pos.y, TILE_SIZE);
            Rectangle rec = (Rectangle){.x = x_index * (TILE_SIZE),
                                        .y = y_index * (TILE_SIZE),
                                        .width = (TILE_SIZE),
                                        .height = (TILE_SIZE)};
            bool interaction_in_range =
                abs((int)game.player.box.x - x_index * TILE_SIZE) <
                    CONFIG.interaction_range * TILE_SIZE &&
                abs((int)game.player.box.y - y_index * TILE_SIZE) <
                    CONFIG.interaction_range * TILE_SIZE;

            if (!slot_selected && interaction_in_range) {
              rec_draw_outline(&rec, BLUE);
            }

            player_render(&game.player);

            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !slot_selected &&
                interaction_in_range) {
              TileInstance *selected_tile =
                  world_tile_at(&game.world, vec2i(x_index, y_index));
              if (CheckCollisionPointRec(mouse_world_pos, selected_tile->box)) {
                if (game.player.held_item.type.id == ITEM_HAMMER) {
                  for (int y = -1; y <= 1; y++) {
                    for (int x = -1; x <= 1; x++) {
                      world_remove_tile(&game.world, vec2i(x_index + x, y_index + y));
                    }
                  }
                } else {
                  world_remove_tile(&game.world, vec2i(x_index, y_index));
                }
              }
            }

            if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && !slot_selected &&
                interaction_in_range) {
              TileInstance *selected_tile =
                  world_tile_at(&game.world, vec2i(x_index, y_index));
              if (CheckCollisionPointRec(mouse_world_pos, selected_tile->box)) {
                TileInstance new_tile =
                    tile_new(&TILES[selected_tile_to_place],
                             x_index * TILE_SIZE, y_index * TILE_SIZE);
                bool placed = world_set_tile(&game.world,
                                             vec2i(x_index, y_index), new_tile);
                if (placed && sound_timer >= SOUND_COOLDOWN) {
                  PlaySound(sound_buffer[cur_sound++]);
                  if (cur_sound >= SOUND_BUFFER_LIMIT) {
                    cur_sound = 0;
                  }
                  sound_timer = 0;
                }
              }
            }

            if (IsKeyReleased(KEYBINDS.open_close_save_menu_key)) {
              if (game.cur_menu == MENU_SAVE) {
                game.cur_menu = MENU_NONE;
                game.paused = false;
              } else {
                game.cur_menu = MENU_SAVE;
                game.paused = true;
              }
            }
          }

          if (IsKeyPressed(KEY_F1)) {
            world_add_being(
                &game.world,
                being_new(
                    BEING_ITEM,
                    (BeingInstanceEx){
                        .type = BEING_INSTANCE_ITEM,
                        .var = {.item_instance = {.item =
                                                      game.player.held_item}}},
                    game.player.box.x, game.player.box.y, 16, 16));
          }

          // TODO: optimize this
          for (int i = 0; i < game.world.beings_amount; i++) {
            if (CheckCollisionRecs(game.world.beings[i].context.box,
                                   game.player.box)) {
              if (GetTime() - game.world.beings[i].context.creation_time >
                  CONFIG.item_pickup_delay) {
                world_remove_being(&game.world, &game.world.beings[i]);
              }
            }
          }

          // DrawTexture(water_animation.frames[currentFrame].texture, 0, 0,
          // WHITE); update_gif_animation(&water_animation, GetFrameTime());

          // CAMERA END
        }
        EndMode2D();
      }
      EndTextureMode();

      int keycode = GetKeyPressed();

      if (keycode >= 48 && keycode <= 57) {
        int tile_index = keycode - 48;
        if (tile_index < TILES_AMOUNT) {
          selected_tile_to_place = tile_index;
          selected_tile_to_place_instance =
              tile_new(&TILES[selected_tile_to_place],
                       selected_tile_to_place_render_pos.x + 35,
                       selected_tile_to_place_render_pos.y - 60);
        }
      }

      if (game.cur_menu != MENU_START) {
        BeginShaderMode(shader);
        {
          DrawTextureRec(world_texture.texture,
                         (Rectangle){0, 0, (float)world_texture.texture.width,
                                     -(float)world_texture.texture.height},
                         (Vector2){0, 0}, WHITE);
        }
        EndShaderMode();

        Vec2i pos = vec2i(SCREEN_WIDTH - (3.5 * 16) - 30,
                          (SCREEN_HEIGHT / 2.0f) - (3.5 * 8));
        DrawTextureEx(slot_texture, (Vector2){pos.x, pos.y}, 0, 4.5, WHITE);
        item_render(&game.player.held_item, pos.x + 2 * 3.5, pos.y + 2 * 3.5);

        tile_render_scaled(&selected_tile_to_place_instance, 4);
      }

      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && slot_selected) {
      }

      menu_render(&ui_renderer, &game);

      if (game.player.held_item.type.id != ITEM_EMPTY) {
        HideCursor();
        // item_render(&game.player.held_item, mousePos.x - 8 * 3.5,
        //             mousePos.y - 8 * 3.5);
        float scale = 3;
        DrawTextureEx(
            cursor_texture,
            (Vector2){.x = mousePos.x - 2 * scale, .y = mousePos.y - 1 * scale},
            0, scale, WHITE);
      } else {
        ShowCursor();
      }
    }
    EndDrawing();

    for (int i = 0; i < TILES_AMOUNT; i++) {
      update_animation(&TILES[i], GetFrameTime());
    }

    TileInstance *tile_under_player =
        world_tile_at(&game.world, game.player.tile_pos);
    if (tile_under_player->type.id != TILE_EMPTY) {
      TraceLog(LOG_DEBUG, "Tile under player: %s",
               tile_type_to_string(&tile_under_player->type));
    }

    TraceLog(LOG_INFO, "Beings amount: %d", game.world.beings_amount);
  }

  // animation_unload(&water_animation);

  UnloadShader(shader);
  UnloadSound(place_sound);

  game_unload(&game);

  CloseAudioDevice();
  CloseWindow();
}
