#include "../include/game.h"
#include "../include/shared.h"
#include "../include/stb_perlin.h"
#include "raylib.h"

#define TILE_SIZE 16

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ballz");
  SetExitKey(0);

  Game game = {
      .player = player_new(),
      .world = world_new(),
  };

  if (FileExists("bytes.bin")) {
    uint8_t bytes[4000];
    ByteBuf buf = {
        .bytes = bytes, .writer_index = 0, .reader_index = 0, .capacity = 4000};
    byte_buf_from_file(&buf);
    load_game(&game, &buf);
  }

  TraceLog(LOG_INFO, "Essence: %d", game.player.essence);

  SetTargetFPS(60);

  Texture2D grass_texture = load_texture("assets/grass.png");
  Texture2D stone_texture = load_texture("assets/stone.png");

  while (!WindowShouldClose()) {
    game_tick(&game);

    BeginDrawing();
    ClearBackground(RAYWHITE);
    // CAMERA
    BeginMode2D(game.player.cam);
    for (int y = 0; y < 40; y++) {
      for (int x = 0; x < 40; x++) {
        float fx = x * 0.1;
        float fy = y * 0.1;
        float noise = stb_perlin_noise3(fx, fy, 0.0f, 0, 0, 0);
        float i = (((noise + 1.0f) / 2.0f * 2.50f) / 2.50f);
        // TraceLog(LOG_INFO, "I: %d", i);
        int blue = i * 255;
        // TraceLog(LOG_INFO, "BLUE: %d", blue);

        Texture2D texture;
        Color tint = WHITE;
        if (blue > 127) {
          if (blue < 150) {
            tint.r -= 40;
            tint.g -= 40;
            tint.b -= 40;
          }
          texture = grass_texture;
        } else {
          texture = stone_texture;
        }

        DrawTexture(texture, x * TILE_SIZE, y * TILE_SIZE, tint);
      }
    }

    Camera2D *cam = &game.player.cam;

    Texture2D texture = player_get_texture(&game.player);
    TraceLog(LOG_INFO, "Direction: %d", game.player.direction);
    DrawTexture(texture, cam->target.x, cam->target.y, WHITE);
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
