#pragma once

#include "data.h"
#include "chunk.h"

typedef struct {
  Chunk chunks[WORLD_CHUNK_CAPACITY][WORLD_CHUNK_CAPACITY];
} World;

World world_new();

void world_gen(World *world);

void world_prepare_rendering(World *world);

void world_render(World *world);

void load_world(World *world, DataMap *data);

void save_world(World *world, DataMap *data);
