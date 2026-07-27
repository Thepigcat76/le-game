#include "../../include/data/data_file.h"
#include "../../include/data/load.h"
#include "../../include/data/save.h"
#include "../../include/game.h"
#include "../../include/net/client.h"
#include "lilc/array.h"
#include "lilc/log.h"
#include "lilc/panic.h"
#include <lilc/alloc.h>

// GAME LOAD/SAVE

#define SAVE_DATA(save_desc, save_file_name, byte_buf_size, byte_buf_name, ...)                                                            \
  {                                                                                                                                        \
    uint8_t *byte_buf_name##_bytes = (uint8_t *)malloc(byte_buf_size);                                                                     \
    ByteBuf byte_buf_name = {.bytes = byte_buf_name##_bytes, .writer_index = 0, .reader_index = 0, .capacity = byte_buf_size};             \
    __VA_ARGS__                                                                                                                            \
    char path[256];                                                                                                                        \
    if (!save_desc.is_server_save) {                                                                                                       \
      strcpy(path, TextFormat("save/save%d/%s.bin", save_desc.id, save_file_name));                                                        \
    } else {                                                                                                                               \
      strcpy(path, TextFormat("server-save/%s.bin", save_file_name));                                                                      \
    }                                                                                                                                      \
    byte_buf_to_file(&byte_buf_name, path);                                                                                                \
    TraceLog(LOG_INFO, "Saved %s at %s data, writer index: %d", save_file_name, path, byte_buf_name.writer_index);                         \
    free(byte_buf_name##_bytes);                                                                                                           \
  }

#define LOAD_DATA(save_desc, save_file_name, byte_buf_size, byte_buf_name, ...)                                                            \
  {                                                                                                                                        \
    uint8_t *byte_buf_name##_bytes = (uint8_t *)malloc(byte_buf_size);                                                                     \
    ByteBuf byte_buf_name = {.bytes = byte_buf_name##_bytes, .writer_index = 0, .reader_index = 0, .capacity = byte_buf_size};             \
    char path[256];                                                                                                                        \
    if (!save_desc.is_server_save) {                                                                                                       \
      strcpy(path, TextFormat("save/save%d/%s.bin", save_desc.id, save_file_name));                                                        \
    } else {                                                                                                                               \
      strcpy(path, TextFormat("server-save/%s.bin", save_file_name));                                                                      \
    }                                                                                                                                      \
    byte_buf_from_file(&byte_buf_name, path);                                                                                              \
    __VA_ARGS__ TraceLog(LOG_INFO, "Loaded %s data, reader index: %d", save_file_name, byte_buf_name.reader_index);                        \
    free(byte_buf_name##_bytes);                                                                                                           \
  }

// LOAD

void game_load_save_data(Game *game, SaveDescriptor save_desc) {
  DataContext ctx = {.registries = &game->registries};

  Save save = save_new(save_desc);

  //  if (save_desc.is_server_save) {
  //    size_t saved_players = 0;
  //    DIR_ITER("server-save/players", entry, {
  //      if (strncmp(entry->d_name, "player-", strlen("player-")) == 0) {
  //        saved_players++;
  //      }
  //    });
  //
  //    game->client_game->cur_player;
  //
  //    for (size_t i = 0; i < saved_players; i++) {
  //      char dir_buf[256];
  //      sprintf(dir_buf, "players/player-%zu", i);
  //      LOAD_DATA(save_desc, dir_buf, sizeof(Player), byte_buf, {
  //        Data data_map = byte_buf_read_data(&byte_buf);
  //        DataMap *player_map = &data_map.var.data_map;
  //        player_load(&save.players[i], player_map);
  //        data_free(&data_map);
  //      });
  //    }
  //  } else {
  // LOAD_DATA(save_desc, "player", 2048, byte_buf, {
  //  Data data_map = byte_buf_read_data(&byte_buf);
  //  DataMap *player_map = &data_map.var.data_map;
  //  Player player = {0};
  //  player_init(&player);
  //
  //  player_load(&player, player_map, ctx);
  //  game->client_game->cur_player = player;
  ////    data_free(&data_map);
  //});
  //  }

  DataMap player_data;

  const char *path = str_fmt_temp("save/save%d/player.bin", save_desc.id);
  data_file_read(path, NULL, &player_data, &HEAP_ALLOCATOR);

  Player player = {0};
  player_init(&player);

  player_load(&player, &player_data, ctx);

  game->client_game->cur_player = player;

  // Scan for the save's spaces
  save_scan_spaces(&save, (DataContext){.registries = &game->registries});

  if (array_len(save.spaces) == 0)
    panic("Failed to get spaces, none exist :(");

  SpaceDescriptor cur_space = {.space_id = SPACE_BASE};

  SpaceDescriptor *desc;
  array_foreach(save.spaces, desc) {
    if (desc->space_id == player.cur_space) {
      cur_space = *desc;
      break;
    }
  }

  Space space;
  space_init(&space, cur_space, save_desc.config.seed);
  space_load(save_desc, space.desc, &space, ctx);
  array_add(save.loaded_spaces, space);

  game->cur_save = save;
  game->client_game->world = &game->cur_save.loaded_spaces[0].world;
  // game->cur_save.cur_space = &game->cur_save.loaded_spaces[0];
}

// UNLOAD

void game_save_save_data(Game *game, Save *save) {
  DataContext ctx = {.registries = &game->registries};

  // if (save->descriptor.is_server_save) {
  //   for (size_t i = 0; i < 1 /*array_len(save->players)*/; i++) {
  //     char dir_buf[256];
  //     sprintf(dir_buf, "players/player-%zu", i);
  //     SAVE_DATA(save->descriptor, dir_buf, 2048, byte_buf, {
  //       DataMap player_map = data_map_new(200);
  //       // player_save(&save->players[i], &player_map);
  //       player_save(&game->client_game->cur_player, &player_map, ctx);
  //
  //      Data player_data = data_map(player_map);
  //      byte_buf_write_data(&byte_buf, &player_data);
  //
  //      data_free(&player_data);
  //    });
  //  }
  //} else {
  //  SAVE_DATA(save->descriptor, "player", 2048, byte_buf, {
  //    log_debug("Bytebuf writer index: %zu", byte_buf.writer_index);
  //
  //    DataMap player_map = data_map_new(200);
  //    // player_save(&save->players[0], &player_map);
  //    player_save(&game->client_game->cur_player, &player_map, ctx);
  //
  //    Data player_data = data_map(player_map);
  //    byte_buf_write_data(&byte_buf, &player_data);
  //
  //    data_free(&player_data);
  //  });
  //}

  if (!save->descriptor.is_server_save) {
    DataMap player_map = data_map_new(200, &HEAP_ALLOCATOR);

    player_save(&game->client_game->cur_player, &player_map, ctx);

    DataHeader header = {.version = 0};
    const char *path = str_fmt_temp("save/save%d/player.bin", save->descriptor.id);
    data_file_write(path, &header, &player_map, &HEAP_ALLOCATOR);
  }

  size_t loaded_saves_len = array_len(game->cur_save.loaded_spaces);
  for (size_t i = 0; i < loaded_saves_len; i++) {
    space_save(save->descriptor, &game->cur_save.loaded_spaces[i], ctx);
  }
}
