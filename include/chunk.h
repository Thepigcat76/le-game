#pragma once

#include "data.h"
#include "tile.h"
#include "shared.h"

typedef struct {
  ChunkPos chunk_pos;
  TileInstance tiles[CHUNK_SIZE][CHUNK_SIZE][TILE_LAYERS_AMOUNT];
  int variant_index;
  int background_texture_variants[CHUNK_SIZE][CHUNK_SIZE];
} Chunk;

void chunk_gen(Chunk *chunk, ChunkPos chunk_pos);

bool chunk_can_place_tile(Chunk *chunk, TileInstance tile, int x, int y);

bool chunk_set_tile(Chunk *chunk, TileInstance tile, int x, int y, TileLayer layer);

void chunk_load(Chunk *chunk, const DataMap *data);

void chunk_save(const Chunk *chunk, DataMap *data);
