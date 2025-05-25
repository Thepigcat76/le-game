#pragma once

#include "being.h"
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

typedef struct world {
  Chunk *chunks;
  size_t chunks_amount;
  ChunkLookup chunk_lookup;
  struct being_instance beings[MAX_ENTITIES_AMOUNT];
  int beings_amount;
  float seed;
  bool initialized;
} World;

World world_new();

void world_initialize(World *world);

bool world_has_chunk_at(const World *world, Vec2i chunk_pos);

void world_add_chunk(World *world, Vec2i pos, Chunk chunk);

ssize_t world_chunk_index_by_pos(const World *world, Vec2i pos);

TileInstance *world_ground_tile_at(World *world, TilePos tile_pos);

TileInstance *world_highest_tile_at(World *world, TilePos tile_pos);

TileInstance *world_tile_at(World *world, TilePos tile_pos, TileLayer layer);

void world_gen(World *world);

void world_gen_chunk_at(World *world, Vec2i chunk_pos);

bool world_set_tile(World *world, TilePos tile_pos, TileInstance tile);

bool world_set_tile_on_layer(World *world, TilePos tile_pos, TileInstance tile, TileLayer layer);

bool world_place_tile(World *world, TilePos tile_pos, TileInstance tile);

bool world_remove_tile(World *world, TilePos tile_pos);

void world_prepare_rendering(World *world);

void world_prepare_chunk_rendering(World *world, Chunk *chunk);

void world_set_tile_texture_data(World *world, TileInstance *tile, int x, int y);

void world_render_layer(World *world, TileLayer layer);

void world_render_layer_top_split(World *world, void *player, bool draw_before_player);

void world_add_being(World *world, BeingInstance being);

// TODO: Use UUID at some point
void world_remove_being(World *world, BeingInstance *being);

void load_world(World *world, const DataMap *data);

void save_world(const World *world, DataMap *data);
