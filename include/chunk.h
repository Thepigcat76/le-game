#pragma once

#include "data.h"
#include "tile.h"
#include "shared.h"

typedef struct {
  TileInstance tiles[CHUNK_SIZE][CHUNK_SIZE];
} Chunk;

void chunk_load(Chunk *chunk, DataMap *data);

void chunk_save(Chunk *chunk, DataMap *data);
