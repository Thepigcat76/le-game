#include "../include/chunk.h"
#include "stdio.h"
#include <raylib.h>
#include <string.h>

void chunk_set_tile(Chunk *chunk, TileInstance tile, int x, int y) {
  if (chunk->tiles[y][x].type.id == tile.type.id) {
    return; // No need to update if the tile is the same
  }

  if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE) {
    fprintf(stderr, "Error: Chunk coordinates out of bounds\n");
    return;
  }

  chunk->tiles[y][x] = tile;

  // Loop over the surrounding tiles (including center)
  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      int nx = x + dx;
      int ny = y + dy;

      // Check bounds
      if (nx >= 0 && nx < CHUNK_SIZE && ny >= 0 && ny < CHUNK_SIZE) {
        chunk_set_tile_texture_data(chunk, nx, ny);
        tile_calc_sprite_box(&chunk->tiles[ny][nx]);
      }
    }
  }
}

void chunk_set_tile_texture_data(Chunk *chunk, int x, int y) {
  TileTextureData *texture_data = &chunk->tiles[y][x].texture_data;
  texture_data->surrounding_tiles[0] =
      y > 0 && x > 0 ? chunk->tiles[y - 1][x - 1].type.id : TILE_EMPTY;
  texture_data->surrounding_tiles[1] =
      y > 0 ? chunk->tiles[y - 1][x].type.id : TILE_EMPTY;
  texture_data->surrounding_tiles[2] = y > 0 && x < CHUNK_SIZE - 1
                                           ? chunk->tiles[y - 1][x + 1].type.id
                                           : TILE_EMPTY;
  texture_data->surrounding_tiles[3] =
      x > 0 ? chunk->tiles[y][x - 1].type.id : TILE_EMPTY;
  texture_data->surrounding_tiles[4] =
      x < CHUNK_SIZE - 1 ? chunk->tiles[y][x + 1].type.id : TILE_EMPTY;
  texture_data->surrounding_tiles[5] = y < CHUNK_SIZE - 1 && x > 0
                                           ? chunk->tiles[y + 1][x - 1].type.id
                                           : TILE_EMPTY;
  texture_data->surrounding_tiles[6] =
      y < CHUNK_SIZE - 1 ? chunk->tiles[y + 1][x].type.id : TILE_EMPTY;
  texture_data->surrounding_tiles[7] = y < CHUNK_SIZE - 1 && x < CHUNK_SIZE - 1
                                           ? chunk->tiles[y + 1][x + 1].type.id
                                           : TILE_EMPTY;
}

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
