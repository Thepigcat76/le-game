#pragma once

#include "chunk.h"
#include "data.h"
#include "shared.h"
#include "tile.h"
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

TileInstance *world_tile_at(World *world, TilePos tile_pos) ;

void world_gen(World *world);

void world_gen_chunk_at(World *world, Vec2i chunk_pos);

bool world_set_tile(World *world, TilePos tile_pos, TileInstance tile);

void world_prepare_rendering(World *world);

void world_prepare_chunk_rendering(World *world, Chunk *chunk);

void world_set_tile_texture_data(World *world, TileInstance *tile, int x, int y);

void world_render(World *world);

void load_world(World *world, const DataMap *data);

void save_world(const World *world, DataMap *data);
