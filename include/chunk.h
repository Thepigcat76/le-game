#pragma once

#include "data.h"
#include "tile.h"
#include "shared.h"
#include "world_type.h"

typedef struct {
  const WorldType *world_type;
  ChunkPos chunk_pos;
  TileInstance tiles[CHUNK_SIZE][CHUNK_SIZE][TILE_LAYERS_AMOUNT];
  int variant_index;
  int background_texture_variants[CHUNK_SIZE][CHUNK_SIZE];
} Chunk;

void chunk_gen(Chunk *chunk, ChunkPos chunk_pos, float world_seed);

bool chunk_can_place_tile_on_layer(Chunk *chunk, TileInstance tile, int x, int y, TileLayer layer);

bool chunk_set_tile(Chunk *chunk, TileInstance tile, int x, int y, TileLayer layer);

void chunk_load(Chunk *chunk, const DataMap *data);

void chunk_save(const Chunk *chunk, DataMap *data);
