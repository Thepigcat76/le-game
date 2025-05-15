#include "../include/world.h"
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#define STB_PERLIN_IMPLEMENTATION
#include "../include/stb_perlin.h"
#include <raylib.h>
#include <stdio.h>

World world_new() {
  return (World){
      .chunks_amount = 0,
  };
}

void world_add_chunk(World *world, ChunkPos pos, Chunk chunk) {
  world->chunks[world->chunks_amount] = chunk;
  world->chunk_lookup.indices[world->chunks_amount] = world->chunks_amount;
  world->chunk_lookup.chunks_positions[world->chunks_amount] = pos;
  world->chunks_amount++;
}

ssize_t world_chunk_index_by_pos(const World *world, ChunkPos pos) {
  for (size_t i = 0; i < world->chunks_amount; i++) {
    if (vec2_eq(&world->chunk_lookup.chunks_positions[i], &pos)) {
      return world->chunk_lookup.indices[i];
    }
  }
  return -1;
}

bool world_has_chunk_at(const World *world, Vec2i chunk_pos) {
  ssize_t index = world_chunk_index_by_pos(world, chunk_pos);
  return index != -1;
}

void world_gen_chunk_at(World *world, Vec2i chunk_pos) {
  Chunk chunk;

  chunk_gen(&chunk, chunk_pos);

  world_add_chunk(world, chunk_pos, chunk);
}

TileInstance *world_tile_at(World *world, TilePos tile_pos) {
  int chunk_tile_x = tile_pos.x % CHUNK_SIZE;
  int chunk_tile_y = tile_pos.y % CHUNK_SIZE;
  int chunk_x = (tile_pos.x - chunk_tile_x) / CHUNK_SIZE;
  int chunk_y = (tile_pos.y - chunk_tile_y) / CHUNK_SIZE;
  ChunkPos chunk_pos = vec2i(chunk_x, chunk_y);
  if (world_has_chunk_at(world, chunk_pos)) {
    Chunk *chunk = &world->chunks[world_chunk_index_by_pos(world, chunk_pos)];
    return &chunk->tiles[chunk_tile_y][chunk_tile_x];
  }
  return &TILE_INSTANCE_EMPTY;
}

void world_gen(World *world) { world_gen_chunk_at(world, vec2i(0, 0)); }

void world_set_tile(World *world, TilePos tile_pos, TileInstance tile) {
  int chunk_tile_x = tile_pos.x % CHUNK_SIZE;
  int chunk_tile_y = tile_pos.y % CHUNK_SIZE;
  int chunk_x = (tile_pos.x - chunk_tile_x) / CHUNK_SIZE;
  int chunk_y = (tile_pos.y - chunk_tile_y) / CHUNK_SIZE;
  ChunkPos chunk_pos = vec2i(chunk_x, chunk_y);
  if (world_has_chunk_at(world, chunk_pos)) {
    Chunk *chunk = &world->chunks[world_chunk_index_by_pos(world, chunk_pos)];
    bool success = chunk_set_tile(chunk, tile, chunk_tile_x, chunk_tile_y);

    if (success) { // Loop over the surrounding tiles (including center)
      for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
          int nx = chunk_tile_x + dx;
          int ny = chunk_tile_y + dy;
          TilePos pos = vec2i((chunk->chunk_pos.x * CHUNK_SIZE) + nx,
                              (chunk->chunk_pos.y * CHUNK_SIZE) + ny);

          // Check bounds
          world_set_tile_texture_data(world, world_tile_at(world, pos), pos.x,
                                      pos.y);
        }
      }

      for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
          int nx = chunk_tile_x + dx;
          int ny = chunk_tile_y + dy;
          TilePos pos = vec2i((chunk->chunk_pos.x * CHUNK_SIZE) + nx,
                              (chunk->chunk_pos.y * CHUNK_SIZE) + ny);

          // Check bounds
          tile_calc_sprite_box(world_tile_at(world, pos));
        }
      }
    }
  }
}

void world_prepare_chunk_rendering(World *world, Chunk *chunk) {
  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      TilePos pos = vec2i((chunk->chunk_pos.x * CHUNK_SIZE) + x,
                          (chunk->chunk_pos.y * CHUNK_SIZE) + y);
      TileInstance *tile = world_tile_at(world, vec2i(pos.x, pos.y));
      if (tile->type.id != TILE_EMPTY) {
        world_set_tile_texture_data(world, tile, pos.x, pos.y);
      }
    }
  }

  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      TilePos pos = vec2i((chunk->chunk_pos.x * CHUNK_SIZE) + x,
                          (chunk->chunk_pos.y * CHUNK_SIZE) + y);
      TileInstance *tile = world_tile_at(world, vec2i(pos.x, pos.y));
      if (tile->type.id != TILE_EMPTY) {
        tile_calc_sprite_box(tile);
      }
    }
  }
}

void world_set_tile_texture_data(World *world, TileInstance *tile, int x,
                                 int y) {
  TileTextureData *texture_data = &tile->texture_data;
  texture_data->surrounding_tiles[0] =
      world_tile_at(world, vec2i(x - 1, y - 1))->type.id;
  texture_data->surrounding_tiles[1] =
      world_tile_at(world, vec2i(x, y - 1))->type.id;
  texture_data->surrounding_tiles[2] =
      world_tile_at(world, vec2i(x + 1, y - 1))->type.id;
  texture_data->surrounding_tiles[3] =
      world_tile_at(world, vec2i(x - 1, y))->type.id;
  texture_data->surrounding_tiles[4] =
      world_tile_at(world, vec2i(x + 1, y))->type.id;
  texture_data->surrounding_tiles[5] =
      world_tile_at(world, vec2i(x - 1, y + 1))->type.id;
  texture_data->surrounding_tiles[6] =
      world_tile_at(world, vec2i(x, y + 1))->type.id;
  texture_data->surrounding_tiles[7] =
      world_tile_at(world, vec2i(x + 1, y + 1))->type.id;
}

void world_prepare_rendering(World *world) {
  for (int i = 0; i < world->chunks_amount; i++) {
    world_prepare_chunk_rendering(world, &world->chunks[i]);
  }
}

void world_render(const World *world) {
  for (int i = 0; i < world->chunks_amount; i++) {
    Vec2i chunk_pos = world->chunk_lookup.chunks_positions[i];
    int chunk_x = chunk_pos.x * CHUNK_SIZE;
    int chunk_y = chunk_pos.y * CHUNK_SIZE;
    const Chunk *chunk = &world->chunks[i];
    for (int y = chunk_y; y < chunk_y + CHUNK_SIZE; y++) {
      for (int x = chunk_x; x < chunk_x + CHUNK_SIZE; x++) {
        DrawTexture(TILES[TILE_DIRT].texture, x * TILE_SIZE, y * TILE_SIZE,
                    WHITE);
      }
    }
    for (int y = 0; y < CHUNK_SIZE; y++) {
      for (int x = 0; x < CHUNK_SIZE; x++) {
        const TileInstance *tile = &chunk->tiles[y][x];
        tile_render(tile);
      }
    }
  }
}

void load_world(World *world, const DataMap *data) {
  uint8_t chunks = data_map_get(data, "len").var.data_byte;
  for (int i = 0; i < chunks; i++) {
    char key[2];
    snprintf(key, 2, "%u", i);
    Data data_map = data_map_get(data, key);
    DataMap map = data_map.var.data_map;
    Chunk chunk;
    chunk_load(&chunk, &map);
    world_add_chunk(world, chunk.chunk_pos, chunk);
  }
}

// TODO: Dealloc... pretty much everything
void save_world(const World *world, DataMap *data) {
  data_map_insert(data, "len", data_byte((uint8_t)world->chunks_amount));
  for (int i = 0; i < world->chunks_amount; i++) {
    DataMap map = data_map_new(300);
    const Chunk *chunk = &world->chunks[i];
    chunk_save(chunk, &map);
    char key[2];
    snprintf(key, 2, "%u", i);
    data_map_insert(data, key, data_map(map));
  }
}
