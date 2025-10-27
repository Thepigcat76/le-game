#pragma once

#include "being.h"
#include "chunk.h"
#include "data.h"
#include "save_desc.h"
#include "shared.h"
#include "space_desc.h"
#include "tile.h"
#include "world_type.h"
#include <stddef.h>
#include <unistd.h>

typedef struct {
  Vec2i *chunks_positions;
  size_t *indices;
} ChunkLookup;

typedef struct _world {
  const WorldType *type;
  const SaveDescriptor *save_desc;
  Chunk *chunks;
  ChunkLookup chunk_lookup;
  // TODO: Use dynamic array for beings
  struct _being_instance *beings;
  int beings_amount;
  bool initialized;
  bool clientside;
  float seed;
} World;

World world_new_no_chunks(bool clientside);

World world_new(const WorldType *world_type, float seed);

void world_initialize(World *world);

bool world_has_chunk_at(const World *world, Vec2i chunk_pos);

void world_add_chunk(World *world, Vec2i pos, Chunk chunk);

ssize_t world_chunk_index_by_pos(const World *world, Vec2i pos);

Chunk *world_chunk_at(const World *world, ChunkPos tile_pos);

TileInstance *world_ground_tile_at(const World *world, TilePos tile_pos);

TileInstance *world_highest_tile_at(const World *world, TilePos tile_pos);

TileInstance *world_tile_at(const World *world, TilePos tile_pos, TileLayer layer);

void world_gen(World *world);

void world_gen_chunk_at(World *world, Vec2i chunk_pos);

bool world_set_tile(World *world, TilePos tile_pos, TileInstance tile);

bool world_set_tile_on_layer(World *world, TilePos tile_pos, TileInstance tile, TileLayer layer);

bool world_place_tile(World *world, TilePos tile_pos, TileInstance tile);

bool world_remove_tile(World *world, TilePos tile_pos);

void world_prepare_rendering(World *world);

void world_prepare_chunk_rendering(World *world, Chunk *chunk);

void world_prepare_chunk_rendering_update_nearby(World *world, Chunk *chunk);

void world_set_tile_texture_data(World *world, TileInstance *tile, int x, int y);

void world_render_layer(World *world, TileLayer layer);

void world_render_layer_top_split(World *world, Rectangle player_box, bool draw_before_player);

void world_add_being(World *world, BeingInstance being);

// TODO: Use UUID at some point
void world_remove_being(World *world, BeingInstance *being);

void world_load(World *world, const DataMap *data);

void world_save(const World *world, DataMap *data);
