#include "../include/chunk.h"
#include "stdio.h"
#include <raylib.h>
#include <string.h>

void chunk_load(Chunk *chunk, DataMap *data) {
  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      unsigned char pos = (unsigned char)x;
      pos = (pos & 0x0F) | (y << 4);
      char id_buf[4];
      snprintf(id_buf, 4, "%u", pos);

      unsigned char tile_id = data_map_get(data, id_buf).var.data_byte;

      chunk->tiles[y][x] =
          tile_new(&TILES[tile_id], x * TILE_SIZE, y * TILE_SIZE);
      // TileInstance *tile = &chunk->tiles[y][x];
      // if (TILES[tile_id].stores_custom_data) {
      //   char custom_data[length + 13 + 1];
      //   sprintf(custom_data, "%s", str);
      //   tile->custom_data = data_map_get(data, custom_data);
      // }
    }
  }
}

void chunk_save(Chunk *chunk, DataMap *data) {
  data_map_insert(data, "test", data_int(chunk->tiles[0][0].type.id));
  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      unsigned char pos = (unsigned char)x;
      pos = (pos & 0x0F) | (y << 4);
      char id_buf[4];
      snprintf(id_buf, 4, "%u", pos);

      TileInstance *tile = &chunk->tiles[y][x];
      // Insert with a duplicated/copy string if needed
      data_map_insert(data, id_buf, data_byte(tile->type.id));

      // if (tile->type.stores_custom_data) {
      //   char custom_data[length + 13 + 1];
      //   sprintf(custom_data, "%s", str);
      //   data_map_insert(data, custom_data, tile->custom_data);
      // }
    }
  }
}
