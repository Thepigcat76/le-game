#include "../include/game.h"
#include <raylib.h>

#define RELOAD(src_file_prefix)                                                \
  extern void src_file_prefix##_on_reload();                                   \
  src_file_prefix##_on_reload();

void game_reload() {
  RELOAD(tile);
  RELOAD(config);
}

void game_tick(Game *game) {
  bool zoom_in = IsKeyDown(KEY_UP);
  bool zoom_out = IsKeyDown(KEY_DOWN);

  player_handle_zoom(&game->player, zoom_in, zoom_out);

  bool w = IsKeyDown(KEY_W);
  bool a = IsKeyDown(KEY_A);
  bool s = IsKeyDown(KEY_S);
  bool d = IsKeyDown(KEY_D);

  player_handle_movement(&game->player, w, a, s, d);
}

void game_free(const Game *game) {}

void load_game(Game *game, ByteBuf *bytebuf) {
  DataMap player_map = byte_buf_read_data(bytebuf).var.data_map;
  load_player(&game->player, &player_map);
  DataMap world_map = byte_buf_read_data(bytebuf).var.data_map;
  load_world(&game->world, &world_map);
}

void save_game(Game *game, ByteBuf *bytebuf) {
  DataMap player_map = data_map_new(200);
  save_player(&game->player, &player_map);
  DataMap world_map = data_map_new(400);
  save_world(&game->world, &world_map);

  Data player_data =
      (Data){.type = DATA_TYPE_MAP, .var = {.data_map = player_map}};
  byte_buf_write_data(bytebuf, &player_data);
  Data world_data =
      (Data){.type = DATA_TYPE_MAP, .var = {.data_map = world_map}};
  byte_buf_write_data(bytebuf, &world_data);

  // data_free(&player_map, DATA_TYPE_MAP);
  // data_free(&world_map, DATA_TYPE_MAP);
}