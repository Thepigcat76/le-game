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
  TraceLog(LOG_INFO, "Chunk index: %zu", world->chunks_amount);
  world->chunks[world->chunks_amount] = chunk;
  world->chunk_lookup.indices[world->chunks_amount] = world->chunks_amount;
  world->chunk_lookup.chunks_positions[world->chunks_amount] = pos;
  world->chunks_amount++;
  for (int i = 0; i < world->chunks_amount; i++) {
    TraceLog(LOG_INFO, "chunk lookup %d: %d, %d", i,
             world->chunk_lookup.chunks_positions[i].x,
             world->chunk_lookup.chunks_positions[i].y);
  }
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

void world_gen(World *world) { world_gen_chunk_at(world, vec2i(0, 0)); }

void world_prepare_rendering(World *world) {
  for (int i = 0; i < world->chunks_amount; i++) {
    Chunk *chunk = &world->chunks[i];
    chunk_prepare_rendering(chunk);
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
  TraceLog(LOG_INFO, "Chunks: %u", chunks);
  for (int i = 0; i < chunks; i++) {
    char key[2];
    snprintf(key, 2, "%u", i);
    DataMap map = data_map_get(data, key).var.data_map;
    Chunk chunk;
    chunk_load(&chunk, &map);
    world_add_chunk(world, chunk.chunk_pos, chunk);
  }
}

// TODO: Dealloc... pretty much everything
void save_world(const World *world, DataMap *data) {
  data_map_insert(data, "len",data_byte((uint8_t) world->chunks_amount));
  for (int i = 0; i < world->chunks_amount; i++) {
    DataMap map = data_map_new(300);
    const Chunk *chunk = &world->chunks[i];
    chunk_save(chunk, &map);
    char key[2];
    snprintf(key, 2, "%u", i);
    data_map_insert(data, key, data_map(map));
  }
}
