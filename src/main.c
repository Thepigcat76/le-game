#include "../include/game.h"
#include "../include/shared.h"
#include "raylib.h"
#include <math.h>

void rec_draw_outline(const Rectangle *rec, Color color) {
  DrawRectangleLinesEx(*rec, 1, color);
}

void debug_rect(Rectangle *rect) {
  TraceLog(LOG_INFO, "Rect{x=%f, y=%f, w=%f, h=%f}", rect->x, rect->y,
           rect->width, rect->height);
}

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ballz");
  SetExitKey(0);

  tile_types_init();
  item_types_init();

  Game game = {
      .player = player_new(),
      .world = world_new(),
  };

  player_set_world(&game.player, &game.world);

  world_gen(&game.world);

  if (FileExists("save/game.bin")) {
    uint8_t bytes[4000];
    ByteBuf buf = {
        .bytes = bytes, .writer_index = 0, .reader_index = 0, .capacity = 4000};
    byte_buf_from_file(&buf, "save/game.bin");
    load_game(&game, &buf);
  }

  world_prepare_rendering(&game.world);

  SetTargetFPS(60);

  float speed = 2.0f;

  Shader shader = LoadShader(NULL, "res/shaders/lighting.fs");
  int light_pos_loc = GetShaderLocation(shader, "lightPos");
  int light_color_loc = GetShaderLocation(shader, "lightColor");
  int light_radius_loc = GetShaderLocation(shader, "lightRadius");

  Texture2D torch_texture = load_texture("res/assets/torch.png");
  Texture2D slot_texture = load_texture("res/assets/slot.png");

  RenderTexture2D world_texture =
      LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

  while (!WindowShouldClose()) {
    game_tick(&game);

    if (IsKeyDown(KEY_R)) {
      shader = LoadShader(NULL, "res/shaders/lighting.fs");
      init_connected_info();
    }

    BeginDrawing();
    {
      Camera2D *cam = &game.player.cam;
      // CAMERA BEGIN
      Vector2 mousePos = GetMousePosition();

      Vector2 mouse_world_pos = GetScreenToWorld2D(mousePos, *cam);
      Vector2 light_pos = {(mousePos.x / GetScreenWidth()),
                           1.0 - (mousePos.y / GetScreenHeight())};

      Vector3 light_color = {1.0f, 1.0f, 0.8f}; // warm white
      float light_radius = 0.08f * cam->zoom * (1.0f + 0.11f * sin(GetTime()));

      SetShaderValue(shader, light_pos_loc, &light_pos, SHADER_UNIFORM_VEC2);
      SetShaderValue(shader, light_color_loc, &light_color,
                     SHADER_UNIFORM_VEC3);
      SetShaderValue(shader, light_radius_loc, &light_radius,
                     SHADER_UNIFORM_FLOAT);
      BeginTextureMode(world_texture);
      {
        BeginMode2D(*cam);
        {
          ClearBackground(DARKGRAY);

          world_render(&game.world);

          Texture2D texture = player_get_texture(&game.player);

          Rectangle tile_box = game.world.chunks[0].tiles[0][0].box;

          // rec_draw_outline(&tile_box, BLUE);

          int x_index = (int)(mouse_world_pos.x / TILE_SIZE);
          int y_index = (int)(mouse_world_pos.y / TILE_SIZE);
          Rectangle rec = (Rectangle){.x = x_index * (TILE_SIZE),
                                      .y = y_index * (TILE_SIZE),
                                      .width = (TILE_SIZE),
                                      .height = (TILE_SIZE)};

          rec_draw_outline(&rec, BLUE);
          DrawTextureEx(texture,
                        (Vector2){game.player.box.x, game.player.box.y}, 0, 1,
                        WHITE);

          // TraceLog(LOG_INFO, "Mouse x: %d, y: %d", x_index * 16, y_index *
          // 16); debug_rect(&tile_rect);

          if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            TileInstance selected_tile =
                game.world.chunks[0].tiles[y_index][x_index];
            if (CheckCollisionPointRec(mouse_world_pos, selected_tile.box)) {
              TileInstance new_tile = tile_new(
                  &TILES[TILE_STONE], x_index * TILE_SIZE, y_index * TILE_SIZE);
              chunk_set_tile(&game.world.chunks[0], new_tile, x_index,
                             y_index);
            }
          }

          //DrawTextureEx(torch_texture,
          //              (Vector2){mouse_world_pos.x, mouse_world_pos.y}, 0, 2,
          //              WHITE);

          // CAMERA END
        }
        EndMode2D();
      }
      EndTextureMode();

      BeginShaderMode(shader);
      {
        DrawTextureRec(world_texture.texture,
                       (Rectangle){0, 0, (float)world_texture.texture.width,
                                   -(float)world_texture.texture.height},
                       (Vector2){0, 0}, WHITE);
      }
      EndShaderMode();

      DrawTextureEx(slot_texture,
                    (Vector2){.x = SCREEN_WIDTH - (3.5 * 16) - 30,
                              .y = (SCREEN_HEIGHT / 2.0f) - (3.5 * 8)},
                    0, 3.5, WHITE);
      ItemInstance item = {
        .type = ITEMS[ITEM_TORCH],
      };
      item_render(&item, mousePos.x, mousePos.y);
    }
    EndDrawing();
  }

  uint8_t bytes[5000];
  ByteBuf buf = {
      .bytes = bytes, .writer_index = 0, .reader_index = 0, .capacity = 5000};
  save_game(&game, &buf);
  byte_buf_to_file(&buf, "save/game.bin");

  CloseWindow();
}