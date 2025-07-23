#include "../../include/array.h"
#include "../../include/game.h"

// GAME LOAD/SAVE

#define SAVE_DATA(save_desc, save_file_name, byte_buf_size, byte_buf_name, ...)                                                            \
  {                                                                                                                                        \
    uint8_t *byte_buf_name##_bytes = (uint8_t *)malloc(byte_buf_size);                                                                     \
    ByteBuf byte_buf_name = {.bytes = byte_buf_name##_bytes, .writer_index = 0, .reader_index = 0, .capacity = byte_buf_size};             \
    __VA_ARGS__ byte_buf_to_file(&byte_buf_name, TextFormat("save/save%d/" save_file_name ".bin", save_desc.id));                          \
    TraceLog(LOG_INFO, "Saved " save_file_name " at %s data, writer index: %d",                                                            \
             TextFormat("save/save%d/" save_file_name ".bin", save_desc.id), byte_buf_name.writer_index);                                  \
    free(byte_buf_name##_bytes);                                                                                                           \
  }

#define LOAD_DATA(save_desc, save_file_name, byte_buf_size, byte_buf_name, ...)                                                            \
  {                                                                                                                                        \
    uint8_t *byte_buf_name##_bytes = (uint8_t *)malloc(byte_buf_size);                                                                     \
    ByteBuf byte_buf_name = {.bytes = byte_buf_name##_bytes, .writer_index = 0, .reader_index = 0, .capacity = byte_buf_size};             \
    byte_buf_from_file(&byte_buf_name, TextFormat("save/save%d/" save_file_name ".bin", save_desc.id));                                    \
    __VA_ARGS__ TraceLog(LOG_INFO, "Loaded " save_file_name " data, reader index: %d", byte_buf_name.reader_index);                        \
    free(byte_buf_name##_bytes);                                                                                                           \
  }

// LOAD

void game_load_save_data(Game *game, SaveDescriptor save_desc) {
  Save save = save_new(save_desc);

  LOAD_DATA(save_desc, "player", sizeof(Player), byte_buf, {
    Data data_map = byte_buf_read_data(&byte_buf);
    DataMap *player_map = &data_map.var.data_map;
    player_load(&save.player, player_map);
    data_free(&data_map);
  });

  save_load_spaces(&save);

  // TODO: Load into the space that the player last played
  Space space;
  space_create_default(save_desc.config.seed, &space);
  space_load(save_desc, space.desc, &space);
  array_add(save.loaded_spaces, space);

  game->cur_save = save;
  game->cur_save.cur_space = &save.loaded_spaces[0];
}

// UNLOAD

void game_save_save_data(Game *game, Save *save) {
  SAVE_DATA(save->descriptor, "player", sizeof(Player), byte_buf, {
    DataMap player_map = data_map_new(200);
    player_save(&save->player, &player_map);

    Data player_data = data_map(player_map);
    byte_buf_write_data(&byte_buf, &player_data);

    data_free(&player_data);
  });

  size_t loaded_saves_len = array_len(game->cur_save.loaded_spaces);
  for (int i = 0; i < loaded_saves_len; i++) {
    space_save(save->descriptor, &game->cur_save.loaded_spaces[i]);
  }

}