#include "../include/game.h"
#include "../include/shared.h"
#include "raylib.h"

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

  Game game = {
      .player = player_new(),
      .world = world_new(),
  };

  world_gen(&game.world);

  world_prepare_rendering(&game.world);

  if (FileExists("bytes.bin")) {
    uint8_t bytes[4000];
    ByteBuf buf = {
        .bytes = bytes, .writer_index = 0, .reader_index = 0, .capacity = 4000};
    byte_buf_from_file(&buf);
    load_game(&game, &buf);
  }

  SetTargetFPS(60);

  float speed = 2.0f;

  while (!WindowShouldClose()) {
    game_tick(&game);

    BeginDrawing();
    ClearBackground(RAYWHITE);
    // CAMERA BEGIN
    BeginMode2D(game.player.cam);
    {
      Camera2D *cam = &game.player.cam;

      world_render(&game.world);

      Texture2D texture = player_get_texture(&game.player);
      DrawTextureEx(texture, (Vector2){game.player.box.x, game.player.box.y}, 0,
                    1, WHITE);

      Vector2 mouse_world_pos = GetScreenToWorld2D(GetMousePosition(), *cam);

      Rectangle tile_box = game.world.chunks[0][0].tiles[0][0].box;

      //rec_draw_outline(&tile_box, BLUE);

      int x_index = (int)(mouse_world_pos.x / TILE_SIZE);
      int y_index = (int)(mouse_world_pos.y / TILE_SIZE);
      Rectangle rec = (Rectangle){.x = x_index * (TILE_SIZE),
                                  .y = y_index * (TILE_SIZE),
                                  .width = (TILE_SIZE),
                                  .height = (TILE_SIZE)};

      rec_draw_outline(&rec, BLUE);

      // TraceLog(LOG_INFO, "Mouse x: %d, y: %d", x_index * 16, y_index * 16);
      // debug_rect(&tile_rect);

      if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        TileInstance *selected_tile =
            &game.world.chunks[0][0].tiles[y_index][x_index];
        if (CheckCollisionPointRec(mouse_world_pos, selected_tile->box)) {
          *selected_tile = tile_new(&TILES[TILE_GRASS], x_index * TILE_SIZE,
                                   y_index * TILE_SIZE);
          world_prepare_rendering(&game.world);
        }
      }
    }
    EndMode2D();

    // CAMERA END
    EndDrawing();
  }

  uint8_t bytes[4000];
  ByteBuf buf = {
      .bytes = bytes, .writer_index = 0, .reader_index = 0, .capacity = 4000};
  save_game(&game, &buf);
  byte_buf_to_file(&buf);

  CloseWindow();
}
