#include "../include/chunk.h"
#include "../include/stb_perlin.h"
#include <stdbool.h>
#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>

void chunk_gen(Chunk *chunk, ChunkPos chunk_pos) {
  int chunk_x = chunk_pos.x * CHUNK_SIZE;
  int chunk_y = chunk_pos.y * CHUNK_SIZE;
  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      float fx = (chunk_x + x) * 0.1;
      float fy = (chunk_y + y) * 0.1;
      int noise = (stb_perlin_noise3(fx, fy, 0.0f, 0, 0, 0) + 1) * 10;

      TileType type;
      if (noise > 5) {
        if (noise < 8) {
          type = TILES[TILE_DIRT];
        } else {
          type = TILES[TILE_GRASS];
        }
      } else {
        type = TILES[TILE_STONE];
      }
      chunk->tiles[y][x] =
          tile_new(&type, (chunk_x + x) * TILE_SIZE, (chunk_y + y) * TILE_SIZE);
    }
  }
  chunk->chunk_pos = chunk_pos;
}

bool chunk_can_place_tile(Chunk *chunk, TileInstance tile, int x, int y) {
  if (chunk->tiles[y][x].type.id == tile.type.id) {
    return false; // No need to update if the tile is the same
  }

  if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE) {
    fprintf(stderr, "Error: Chunk coordinates out of bounds\n");
    return false;
  }
  return true;
}

bool chunk_set_tile(Chunk *chunk, TileInstance tile, int x, int y) {
  if (!chunk_can_place_tile(chunk, tile, x, y)) {
    return false;
  }

  chunk->tiles[y][x] = tile;
  return true;
}

void chunk_load(Chunk *chunk, const DataMap *data) {
  ChunkPos chunk_pos;
  chunk_pos.x = (int)data_map_get(data, "chunk_x").var.data_byte;
  chunk_pos.y = (int)data_map_get(data, "chunk_y").var.data_byte;

  DataList list = data_map_get(data, "tiles").var.data_list;
  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      int8_t id = list.items[y * CHUNK_SIZE + x].var.data_byte;

      chunk->tiles[y][x] =
          tile_new(&TILES[id], (chunk_pos.x * CHUNK_SIZE + x) * TILE_SIZE,
                   (chunk_pos.y * CHUNK_SIZE + y) * TILE_SIZE);
      // TileInstance *tile = &chunk->tiles[y][x];
      // if (TILES[tile_id].stores_custom_data) {
      //   char custom_data[length + 13 + 1];
      //   sprintf(custom_data, "%s", str);
      //   tile->custom_data = data_map_get(data, custom_data);
      // }
    }
  }
  chunk->chunk_pos = chunk_pos;
}

void chunk_save(const Chunk *chunk, DataMap *data) {
  data_map_insert(data, "chunk_x", data_byte(chunk->chunk_pos.x));
  data_map_insert(data, "chunk_y", data_byte(chunk->chunk_pos.y));
  DataList tiles = {.items = malloc(256 * sizeof(Data)), .len = 256};
  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {

      const TileInstance *tile = &chunk->tiles[y][x];
      // Insert with a duplicated/copy string if needed
      tiles.items[y * CHUNK_SIZE + x] = data_byte(tile->type.id);

      // if (tile->type.stores_custom_data) {
      //   char custom_data[length + 13 + 1];
      //   sprintf(custom_data, "%s", str);
      //   data_map_insert(data, custom_data, tile->custom_data);
      // }
    }
  }
  Data data_list_0 = data_list(tiles);
  data_map_insert(data, "tiles", data_list_0);
}
