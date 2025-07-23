#include "../../include/chunk.h"
#ifndef _WIN32
#define STB_PERLIN_IMPLEMENTATION
#endif
#include "../../vendor/stb_perlin.h"
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>

static void chunk_assign_dirt_variants(Chunk *chunk) {
  chunk->variant_index = tile_variants_index_for_name("res/assets/dirt.png", 0, 0);
  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      chunk->background_texture_variants[y][x] = GetRandomValue(0, tile_variants_amount_by_index(chunk->variant_index, 0, 0) - 1);
    }
  }
}

void chunk_gen(Chunk *chunk, ChunkPos chunk_pos, float world_seed) {
  int chunk_x = chunk_pos.x * CHUNK_SIZE;
  int chunk_y = chunk_pos.y * CHUNK_SIZE;
  float seed_offset = world_seed * 37.77f;
  for (int l = 0; l < TILE_LAYERS_AMOUNT; l++) {
    // TraceLog(LOG_INFO, "Generating chunk, layer: %s", l == TILE_LAYER_GROUND ? "Ground" : "Top");
    for (int y = 0; y < CHUNK_SIZE; y++) {
      for (int x = 0; x < CHUNK_SIZE; x++) {
        float fx = (chunk_x + x) * 0.1 + seed_offset;
        float fy = (chunk_y + y) * 0.1 + seed_offset;
        float noise = (stb_perlin_noise3(fx, fy, 0.0f, 0, 0, 0) + 1) * 10.0;
        if (l == TILE_LAYER_GROUND) {
          TileId tile_id;
          if (chunk->world_type->id == WORLD_BASE) {
            if (noise > 5) {
              if (noise < 8) {
                tile_id = TILE_DIRT;
              } else {
                tile_id = TILE_GRASS;
              }
            } else {
              tile_id = TILE_WATER;
            }
          } else if (chunk->world_type->id == WORLD_DUNGEON_TEST) {
            tile_id = TILE_STONE;
          } else {
            PANIC_FMT("NYI World gen for type: %d", chunk->world_type->id);
          }
          chunk->tiles[y][x][l] = tile_new(&TILES[tile_id]);
        } else {
          // if (noise > 9.9) {
          //   chunk->tiles[y][x][l] = tile_new(&TILES[TILE_TREE]);
          // } else {
          chunk->tiles[y][x][l] = tile_new(&TILES[TILE_EMPTY]);
          //}
        }
      }
    }
  }
  chunk_assign_dirt_variants(chunk);
  chunk->chunk_pos = chunk_pos;
}

bool chunk_can_place_tile_on_layer(Chunk *chunk, TileInstance tile, int x, int y, TileLayer layer) {
  if (chunk->tiles[y][x][layer].type->id == tile.type->id) {
    return false; // No need to update if the tile is the same
  }

  if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE) {
    fprintf(stderr, "Error: Chunk coordinates out of bounds: %d, %d\n", x, y);
    return false;
  }
  return true;
}

bool chunk_set_tile(Chunk *chunk, TileInstance tile, int x, int y, TileLayer layer) {
  if (!chunk_can_place_tile_on_layer(chunk, tile, x, y, layer)) {
    return false;
  }

  chunk->tiles[y][x][layer] = tile;
  printf("Set tile on layer: %d\n", layer);
  return true;
}

void chunk_load(Chunk *chunk, const DataMap *data) {
  ChunkPos chunk_pos;
  chunk_pos.x = (int)data_map_get(data, "chunk_x").var.data_int;
  chunk_pos.y = (int)data_map_get(data, "chunk_y").var.data_int;

  for (int l = 0; l < TILE_LAYERS_AMOUNT; l++) {
    DataList list = data_map_get(data, TextFormat("tiles%d", l)).var.data_list;
    for (int y = 0; y < CHUNK_SIZE; y++) {
      for (int x = 0; x < CHUNK_SIZE; x++) {
        int8_t id = list.items[y * CHUNK_SIZE + x].var.data_byte;

        chunk->tiles[y][x][l] = tile_new(&TILES[id]);
        // TileInstance *tile = &chunk->tiles[y][x];
        // if (TILES[tile_id].stores_custom_data) {
        //   char custom_data[length + 13 + 1];
        //   sprintf(custom_data, "%s", str);
        //   tile->custom_data = data_map_get(data, custom_data);
        // }
      }
    }
  }
  chunk_assign_dirt_variants(chunk);
  chunk->chunk_pos = chunk_pos;
}

void chunk_save(const Chunk *chunk, DataMap *data) {
  data_map_insert(data, "chunk_x", data_int(chunk->chunk_pos.x));
  data_map_insert(data, "chunk_y", data_int(chunk->chunk_pos.y));
  for (int l = 0; l < TILE_LAYERS_AMOUNT; l++) {
    DataList tiles = data_list_new(256);
    for (int y = 0; y < CHUNK_SIZE; y++) {
      for (int x = 0; x < CHUNK_SIZE; x++) {

        const TileInstance *tile = &chunk->tiles[y][x][l];
        // Insert with a duplicated/copy string if needed
        data_list_add(&tiles, data_byte(tile->type->id));

        // if (tile->type.stores_custom_data) {
        //   char custom_data[length + 13 + 1];
        //   sprintf(custom_data, "%s", str);
        //   data_map_insert(data, custom_data, tile->custom_data);
        // }
      }
    }
    Data data_list_0 = data_list(tiles);
    char tiles_key[sizeof("tiles") + 1];
    sprintf(tiles_key, "tiles%d", l);
    data_map_insert(data, tiles_key, data_list_0);
  }
}
