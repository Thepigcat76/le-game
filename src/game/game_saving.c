#include "../../include/data/data_reader.h"
#include "../../include/game.h"
#include <stdio.h>

// GAME LOAD/SAVE

#define SAVE_DATA(save_desc, save_file_name, byte_buf_size, byte_buf_name, ...)                                        \
  {                                                                                                                    \
    uint8_t *byte_buf_name##_bytes = (uint8_t *)malloc(byte_buf_size);                                                 \
    ByteBuf byte_buf_name = {                                                                                          \
        .bytes = byte_buf_name##_bytes, .writer_index = 0, .reader_index = 0, .capacity = byte_buf_size};              \
    __VA_ARGS__ byte_buf_to_file(&byte_buf_name, TextFormat("save/save%d/" save_file_name ".bin", save_desc.id));      \
    TraceLog(LOG_INFO, "Saved " save_file_name " data, writer index: %d", byte_buf_name.writer_index);                 \
    free(byte_buf_name##_bytes);                                                                                       \
  }

#define LOAD_DATA(save_desc, save_file_name, byte_buf_size, byte_buf_name, ...)                                        \
  {                                                                                                                    \
    uint8_t *byte_buf_name##_bytes = (uint8_t *)malloc(byte_buf_size);                                                 \
    ByteBuf byte_buf_name = {                                                                                          \
        .bytes = byte_buf_name##_bytes, .writer_index = 0, .reader_index = 0, .capacity = byte_buf_size};              \
    byte_buf_from_file(&byte_buf_name, TextFormat("save/save%d/" save_file_name ".bin", save_desc.id));                \
    __VA_ARGS__ TraceLog(LOG_INFO, "Loaded " save_file_name " data, reader index: %d", byte_buf_name.reader_index);    \
    free(byte_buf_name##_bytes);                                                                                       \
  }

// LOAD

Save game_load_save_data(Game *game, SaveDescriptor desc) {
  Save save = {.descriptor = desc, .player = player_new(), .world = world_new()};
  LOAD_DATA(desc, "player", sizeof(Player), byte_buf, {
    Data data_map = byte_buf_read_data(&byte_buf);
    DataMap *player_map = &data_map.var.data_map;
    player_load(&save.player, player_map);
    data_free(&data_map);
  });

  LOAD_DATA(desc, "world", sizeof(Chunk) * WORLD_LOADED_CHUNKS + 100 + sizeof(BeingInstance) * 200, byte_buf, {
    Data data_map = byte_buf_read_data(&byte_buf);
    char *str = data_reader_read_data(&data_map);
    printf("%s\n", str);
    free(str);
    DataMap *world_map = &data_map.var.data_map;
    load_world(&save.world, world_map);

    data_free(&data_map);
  });

  return save;
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

  SAVE_DATA(save->descriptor, "world", sizeof(Chunk) * WORLD_LOADED_CHUNKS + 100 + sizeof(BeingInstance) * 200,
            byte_buf, {
              DataMap world_map = data_map_new(2000);
              save_world(&save->world, &world_map);

              Data world_data = data_map(world_map);
              byte_buf_write_data(&byte_buf, &world_data);

              data_free(&world_data);
            });
}