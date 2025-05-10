#pragma once

#include "chunk.h"
#include "data.h"
#include "shared.h"
#include <stddef.h>
#include <unistd.h>

typedef struct {
  Vec2i chunks_positions[WORLD_LOADED_CHUNKS];
  size_t indices[WORLD_LOADED_CHUNKS];
} ChunkLookup;

typedef struct {
  Chunk chunks[WORLD_LOADED_CHUNKS];
  size_t chunks_amount;
  ChunkLookup chunk_lookup;
} World;

World world_new();

bool world_has_chunk_at(const World *world, Vec2i chunk_pos);

void world_add_chunk(World *world, Vec2i pos, Chunk chunk);

ssize_t world_chunk_index_by_pos(const World *world, Vec2i pos);

void world_gen(World *world);

void world_gen_chunk_at(World *world, Vec2i chunk_pos);

void world_prepare_rendering(World *world);

void world_render(const World *world);

void load_world(World *world, const DataMap *data);

void save_world(const World *world, DataMap *data);
