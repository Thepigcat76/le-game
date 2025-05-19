#include "../include/game.h"
#include "../include/config.h"
#include <raylib.h>

#define RELOAD(src_file_prefix)                                                \
  extern void src_file_prefix##_on_reload();                                   \
  src_file_prefix##_on_reload();

void game_reload() {
  RELOAD(tile);
  RELOAD(config);
}

void game_tick(Game *game) {
  if (!game->paused) {
    bool zoom_in = IsKeyDown(KEY_UP);
    bool zoom_out = IsKeyDown(KEY_DOWN);

    player_handle_zoom(&game->player, zoom_in, zoom_out);

    bool w = IsKeyDown(KEYBINDS.move_backward_key);
    bool a = IsKeyDown(KEYBINDS.move_left_key);
    bool s = IsKeyDown(KEYBINDS.move_foreward_key);
    bool d = IsKeyDown(KEYBINDS.move_right_key);

    player_handle_movement(&game->player, w, a, s, d);
  }
}

void game_add_being(Game *game, BeingInstance being) {
  game->beings[game->beings_amount++] = being;
}

void game_remove_being(Game *game, BeingInstance *being) {
  bool being_found = false;
  for (int i = 0; i < game->beings_amount; i++) {
    if (&game->beings[i] == being) {
      being_found = true;
    }

    if (being_found) {
      game->beings[i - 1] = game->beings[i];
    }
  }
  game->beings_amount--;
}

static void load_game(Game *game, ByteBuf *bytebuf) {
  Data data_map_0 = byte_buf_read_data(bytebuf);
  DataMap *player_map = &data_map_0.var.data_map;
  player_load(&game->player, player_map);

  Data data_map_1 = byte_buf_read_data(bytebuf);
  DataMap *world_map = &data_map_1.var.data_map;
  load_world(&game->world, world_map);

  data_free(&data_map_0);
  data_free(&data_map_1);
}

static void save_game(Game *game, ByteBuf *bytebuf) {
  DataMap player_map = data_map_new(200);
  player_save(&game->player, &player_map);
  DataMap world_map = data_map_new(400);
  save_world(&game->world, &world_map);

  Data player_data =
      (Data){.type = DATA_TYPE_MAP, .var = {.data_map = player_map}};
  byte_buf_write_data(bytebuf, &player_data);
  Data world_data =
      (Data){.type = DATA_TYPE_MAP, .var = {.data_map = world_map}};
  byte_buf_write_data(bytebuf, &world_data);

  data_free(&player_data);
  data_free(&world_data);
}

void game_load(Game *game) {
  uint8_t bytes[SAVE_DATA_BYTES];
  ByteBuf buf = {.bytes = bytes,
                 .writer_index = 0,
                 .reader_index = 0,
                 .capacity = SAVE_DATA_BYTES};
  byte_buf_from_file(&buf, "save/game.bin");
  load_game(game, &buf);
  TraceLog(LOG_INFO, "Successfully loaded game");
}

void game_unload(Game *game) {
  tile_variants_free();

  uint8_t bytes[SAVE_DATA_BYTES];
  ByteBuf buf = {.bytes = bytes,
                 .writer_index = 0,
                 .reader_index = 0,
                 .capacity = SAVE_DATA_BYTES};
  save_game(game, &buf);
  byte_buf_to_file(&buf, "save/game.bin");

  free(game->world.chunks);
}