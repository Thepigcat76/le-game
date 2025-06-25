#include "../../include/game.h"
#include "../../include/data/data_reader.h"
#include <stdio.h>

// GAME LOAD/SAVE

#define SAVE_DATA(save_file_name, byte_buf_size, byte_buf_name, ...)                                                   \
  {                                                                                                                    \
    uint8_t *byte_buf_name##_bytes = (uint8_t *)malloc(byte_buf_size);                                                 \
    ByteBuf byte_buf_name = {                                                                                          \
        .bytes = byte_buf_name##_bytes, .writer_index = 0, .reader_index = 0, .capacity = byte_buf_size};              \
    __VA_ARGS__ byte_buf_to_file(&byte_buf_name, TextFormat("save/save%d/" save_file_name ".bin", game->cur_save));    \
    TraceLog(LOG_INFO, "Saved " save_file_name " data, writer index: %d", byte_buf_name.writer_index);                 \
    free(byte_buf_name##_bytes);                                                                                       \
  }

#define LOAD_DATA(save_file_name, byte_buf_size, byte_buf_name, ...)                                                   \
  {                                                                                                                    \
    uint8_t *byte_buf_name##_bytes = (uint8_t *)malloc(byte_buf_size);                                                 \
    ByteBuf byte_buf_name = {                                                                                          \
        .bytes = byte_buf_name##_bytes, .writer_index = 0, .reader_index = 0, .capacity = byte_buf_size};              \
    byte_buf_from_file(&byte_buf_name, TextFormat("save/save%d/" save_file_name ".bin", game->cur_save));              \
    __VA_ARGS__ TraceLog(LOG_INFO, "Loaded " save_file_name " data, reader index: %d", byte_buf_name.reader_index);    \
    free(byte_buf_name##_bytes);                                                                                       \
  }

// LOAD

void game_load(Game *game) { game_load_cur_save(game); }

void game_load_cur_save(Game *game) {
  LOAD_DATA("player", sizeof(Player), byte_buf, {
    Data data_map = byte_buf_read_data(&byte_buf);
    DataMap *player_map = &data_map.var.data_map;
    player_load(&game->player, player_map);
    data_free(&data_map);
  });

  LOAD_DATA("world", sizeof(Chunk) * WORLD_LOADED_CHUNKS + 100 + sizeof(BeingInstance) * 200, byte_buf, {
    Data data_map = byte_buf_read_data(&byte_buf);
    char *str = data_reader_read_data(&data_map);
    printf("%s\n", str);
    free(str);
    DataMap *world_map = &data_map.var.data_map;
    load_world(&game->world, world_map);

    data_free(&data_map);
  });
}

// UNLOAD

void game_save_cur_save(Game *game) {
  SAVE_DATA("player", sizeof(Player), byte_buf, {
    DataMap player_map = data_map_new(200);
    player_save(&game->player, &player_map);

    Data player_data = data_map(player_map);
    byte_buf_write_data(&byte_buf, &player_data);

    data_free(&player_data);
  });

  SAVE_DATA("world", sizeof(Chunk) * WORLD_LOADED_CHUNKS + 100 + sizeof(BeingInstance) * 200, byte_buf, {
    DataMap world_map = data_map_new(2000);
    save_world(&game->world, &world_map);

    Data world_data = data_map(world_map);
    byte_buf_write_data(&byte_buf, &world_data);

    data_free(&world_data);
  });
}