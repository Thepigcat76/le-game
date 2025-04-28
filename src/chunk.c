#include "../include/chunk.h"
#include "stdio.h"
#include <raylib.h>
#include <string.h>

void chunk_load(Chunk *chunk, DataMap *data) {
  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      char buf[20];
      int length = sprintf(buf, "%d,%d", x, y);
      char str[length + 1];
      strcpy(str, buf);
      TraceLog(LOG_INFO, "String: %s", str);

      char id[length + 3 + 1];
      sprintf(id, "%s_id", str);
      int tile_id = data_map_get(data, id).var.data_int;
      chunk->tiles[y][x] =
          tile_new(&TILES[tile_id], x * TILE_SIZE, y * TILE_SIZE);
      TileInstance *tile = &chunk->tiles[y][x];
      if (TILES[tile_id].stores_custom_data) {
        char custom_data[length + 13 + 1];
        sprintf(custom_data, "%s", str);
        tile->custom_data = data_map_get(data, custom_data);
      }
    }
  }
}

void chunk_save(Chunk *chunk, DataMap *data) {
  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      char buf[20];
      int length = sprintf(buf, "%d,%d", x, y);
      char str[length + 1];
      strcpy(str, buf);
      TraceLog(LOG_INFO, "String: %s", str);

      TileInstance *tile = &chunk->tiles[y][x];
      char id[length + 3 + 1];
      sprintf(id, "%s_id", str);
      data_map_insert(data, id, data_int(tile->type.id));
      if (tile->type.stores_custom_data) {
        char custom_data[length + 13 + 1];
        sprintf(custom_data, "%s", str);
        data_map_insert(data, custom_data, tile->custom_data);
      }
    }
  }
}
