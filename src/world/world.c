#include "../../include/world.h"
#include "../../include/being.h"
#include "../../include/data/data_reader.h"
#include "../../include/data/load.h"
#include "../../include/data/save.h"
#include "../../include/game.h"
#include "../../include/item.h"
#include "../../include/net/client.h"
#include "../../include/particle.h"
#include "../../include/player.h"
#include "lilc/array.h"
#include "lilc/log.h"
#include <lilc/alloc.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

Texture2D particle_texture;
static bool particle_texture_loaded = false;

void world_init(World *world, SpaceId space_id, float seed) {
  world->chunks = array_new_capacity(Chunk, WORLD_LOADED_CHUNKS, &HEAP_ALLOCATOR);
  world->chunk_lookup = (ChunkLookup){.chunks_positions = array_new_capacity(ChunkPos, WORLD_LOADED_CHUNKS, &HEAP_ALLOCATOR),
                                      .indices = array_new_capacity(size_t, WORLD_LOADED_CHUNKS, &HEAP_ALLOCATOR)};
  world->initialized = false;
  world->space_id = space_id;
  world->seed = seed;
}

void world_initialize(World *world) {
  world->initialized = true;
  world_prepare_rendering(world);
}

void world_add_chunk(World *world, ChunkPos pos, Chunk chunk) {
  array_add(world->chunks, chunk);
  size_t len = array_len(world->chunks) - 1;
  array_add(world->chunk_lookup.indices, len);
  array_add(world->chunk_lookup.chunks_positions, pos);
}

ssize_t world_chunk_index_by_pos(const World *world, ChunkPos pos) {
  size_t len = array_len(world->chunks);
  for (size_t i = 0; i < len; i++) {
    if (vec2_eq(&world->chunk_lookup.chunks_positions[i], &pos)) {
      return world->chunk_lookup.indices[i];
    }
  }
  return -1;
}

bool world_has_chunk_at(const World *world, ChunkPos chunk_pos) {
  ssize_t index = world_chunk_index_by_pos(world, chunk_pos);
  return index != -1;
}

static void world_print_debug_chunk_lookup(const ChunkLookup *lookup) {
  printf("-- Chunk Lookup --\n");
  size_t len = array_len(lookup->chunks_positions);
  for (size_t i = 0; i < len; i++) {
    printf("Chunk %d, %d - %zu\n", lookup->chunks_positions[i].x, lookup->chunks_positions[i].y, lookup->indices[i]);
  }
}

void world_gen_chunk_at(World *world, ChunkPos chunk_pos) {
  Chunk chunk = {.space_id = world->space_id};

  chunk_gen(&chunk, chunk_pos, world->seed);

  world_add_chunk(world, chunk_pos, chunk);

  TraceLog(LOG_INFO, "Added chunk %zu", array_len(world->chunks) - 1);

  world_print_debug_chunk_lookup(&world->chunk_lookup);
}

Chunk *world_chunk_at(const World *world, ChunkPos chunk_pos) {
  if (world_has_chunk_at(world, chunk_pos)) {
    ssize_t index = world_chunk_index_by_pos(world, chunk_pos);
    if (index != -1) {
      return &world->chunks[index];
    }
  }
  return NULL;
}

TileInstance *world_ground_tile_at(const World *world, TilePos tile_pos) { return world_tile_at(world, tile_pos, TILE_LAYER_GROUND); }

TileInstance *world_highest_tile_at(const World *world, TilePos tile_pos) {
  for (int l = TILE_LAYERS_AMOUNT - 1; l >= 0; l--) {
    TileInstance *tile = world_tile_at(world, tile_pos, l);
    if (tile->id != TILE_EMPTY || l == 0) {
      return tile;
    }
  }
  return &TILE_INST_EMPTY;
}

TileInstance *world_tile_at(const World *world, TilePos tile_pos, TileLayer layer) {
  int chunk_tile_x = floor_mod(tile_pos.x, CHUNK_SIZE);
  int chunk_tile_y = floor_mod(tile_pos.y, CHUNK_SIZE);

  int chunk_x = floor_div(tile_pos.x, CHUNK_SIZE);
  int chunk_y = floor_div(tile_pos.y, CHUNK_SIZE);

  ChunkPos chunk_pos = vec2i(chunk_x, chunk_y);

  if (world_has_chunk_at(world, chunk_pos)) {
    Chunk *chunk = &world->chunks[world_chunk_index_by_pos(world, chunk_pos)];
    TileInstance *tile = &chunk->tiles[chunk_tile_y][chunk_tile_x][layer];
    return tile;
  }

  return &TILE_INST_EMPTY;
}

static const Vec2i RENDER_CHUNK_OFFSETS[8] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}};

void world_prepare_chunk_rendering_update_nearby(World *world, Chunk *chunk) {
  world_prepare_chunk_rendering(world, chunk);
  for (int i = 0; i < 4; i++) {
    Vec2i offset = RENDER_CHUNK_OFFSETS[i];
    int index = world_chunk_index_by_pos(world, vec2i(chunk->chunk_pos.x + offset.x, chunk->chunk_pos.y + offset.y));
    if (index != -1) {
      world_prepare_chunk_rendering(world, &world->chunks[index]);
    }
  }
}

void world_gen(World *world) {
  for (int y = -1; y <= 1; y++) {
    for (int x = -1; x <= 1; x++) {
      world_gen_chunk_at(world, vec2i(x, y));
    }
  }
  TraceLog(LOG_INFO, "Generated world");
  world_print_debug_chunk_lookup(&world->chunk_lookup);

  world->initialized = true;
  if (GAME_SIDE == SIDE_CLIENT) {
    world_initialize(world);
  }
}

bool world_set_tile(World *world, TilePos tile_pos, TileInstance tile) {
  TileProperties tile_props = CLIENT_GAME.game.registries.tiles[tile.id];
  return world_set_tile_on_layer(world, tile_pos, tile, tile_props.layer);
}

bool world_set_tile_on_layer(World *world, TilePos tile_pos, TileInstance tile, TileLayer layer) {
  int chunk_tile_x = floor_mod(tile_pos.x, CHUNK_SIZE);
  int chunk_tile_y = floor_mod(tile_pos.y, CHUNK_SIZE);
  int chunk_x = floor_div((tile_pos.x - chunk_tile_x), CHUNK_SIZE);
  int chunk_y = floor_div((tile_pos.y - chunk_tile_y), CHUNK_SIZE);
  ChunkPos chunk_pos = vec2i(chunk_x, chunk_y);
  if (world_has_chunk_at(world, chunk_pos)) {
    Chunk *chunk = &world->chunks[world_chunk_index_by_pos(world, chunk_pos)];
    bool success = chunk_set_tile(chunk, tile, chunk_tile_x, chunk_tile_y, layer);

    if (success) {
      for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
          int nx = chunk_tile_x + dx;
          int ny = chunk_tile_y + dy;
          TilePos pos = vec2i((chunk->chunk_pos.x * CHUNK_SIZE) + nx, (chunk->chunk_pos.y * CHUNK_SIZE) + ny);

          world_set_tile_texture_data(world, world_ground_tile_at(world, pos), pos.x, pos.y);
        }
      }

      for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
          int nx = chunk_tile_x + dx;
          int ny = chunk_tile_y + dy;
          TilePos pos = vec2i((chunk->chunk_pos.x * CHUNK_SIZE) + nx, (chunk->chunk_pos.y * CHUNK_SIZE) + ny);

          // Check bounds
          tile_calc_sprite_box(world_ground_tile_at(world, pos));
        }
      }
    }
    return success;
  }
  return false;
}

bool world_place_tile(World *world, TilePos tile_pos, TileInstance tile) {
  TileProperties tile_props = CLIENT_GAME.game.registries.tiles[tile.id];
  TileLayer layer = tile_props.layer;
  for (int l = layer - 1; l >= 0; l--) {
    if (l <= 0) {
      break;
    }

    if (world_tile_at(world, tile_pos, l)->id == TILE_EMPTY) {
      return false;
    }
  }

  if (world_set_tile_on_layer(world, tile_pos, tile, layer)) {
    Sound sound = sound_by_handle(&CLIENT_GAME.asset_manager, SOUND_PLACE);
    PlaySound(sound);
    return true;
  }
  return false;
}

bool world_remove_tile(World *world, TilePos tile_pos) {
  TileInstance empty_instance = TILE_INST_EMPTY;
  TileInstance *tile = world_highest_tile_at(world, tile_pos);
  TileProperties tile_props = CLIENT_GAME.game.registries.tiles[tile->id];
  Color color = tile_props.tile_color;
  i32 tile_item = tile_props.tile_item;
  if (world_set_tile_on_layer(world, tile_pos, empty_instance, tile_props.layer)) {
    log_debug("setting tile");
    if (tile_item != -1) {
      space_add_being(CLIENT_GAME.space,
                      being_item_new((ItemInstance){.id = tile_item}, (tile_pos.x * TILE_SIZE) + GetRandomValue(-7, 9),
                                     (tile_pos.y * TILE_SIZE) + GetRandomValue(-7, 9)));
    }

    for (int i = 0; i < 5; i++) {
      ParticleInstance *particle = client_emit_particle(
          &CLIENT_GAME, tile_pos.x * TILE_SIZE + GetRandomValue(-9, 14), tile_pos.y * TILE_SIZE + GetRandomValue(-9, 14),
          PARTICLE_TILE_BREAK,
          (ParticleInstanceEx){.type = PARTICLE_INSTANCE_TILE_BREAK, .var = {.tile_break = {.texture = particle_texture, .tint = color}}});
      particle->velocity = vec2f(0, 0);
    }
    return true;
  }
  return false;
}

void world_prepare_chunk_rendering(World *world, Chunk *chunk) {
  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      int chunk_x = chunk->chunk_pos.x * CHUNK_SIZE;
      int chunk_y = chunk->chunk_pos.y * CHUNK_SIZE;
      TilePos pos = vec2i(chunk_x + x, chunk_y + y);
      TileInstance *tile = world_ground_tile_at(world, vec2i(pos.x, pos.y));
      if (tile->id != TILE_EMPTY) {
        world_set_tile_texture_data(world, tile, pos.x, pos.y);
      }
    }
  }

  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      TilePos pos = vec2i((chunk->chunk_pos.x * CHUNK_SIZE) + x, (chunk->chunk_pos.y * CHUNK_SIZE) + y);
      TileInstance *tile = world_ground_tile_at(world, vec2i(pos.x, pos.y));
      if (tile->id != TILE_EMPTY) {
        tile_calc_sprite_box(tile);
      }
    }
  }
}

void world_set_tile_texture_data(World *world, TileInstance *tile, int x, int y) {
  TileTextureData *texture_data = &tile->texture_data;
  texture_data->surrounding_tiles[0] = world_ground_tile_at(world, vec2i(x - 1, y - 1))->id;
  texture_data->surrounding_tiles[1] = world_ground_tile_at(world, vec2i(x, y - 1))->id;
  texture_data->surrounding_tiles[2] = world_ground_tile_at(world, vec2i(x + 1, y - 1))->id;
  texture_data->surrounding_tiles[3] = world_ground_tile_at(world, vec2i(x - 1, y))->id;
  texture_data->surrounding_tiles[4] = world_ground_tile_at(world, vec2i(x + 1, y))->id;
  texture_data->surrounding_tiles[5] = world_ground_tile_at(world, vec2i(x - 1, y + 1))->id;
  texture_data->surrounding_tiles[6] = world_ground_tile_at(world, vec2i(x, y + 1))->id;
  texture_data->surrounding_tiles[7] = world_ground_tile_at(world, vec2i(x + 1, y + 1))->id;
}

void world_prepare_rendering(World *world) {
  size_t len = array_len(world->chunks);
  TraceLog(LOG_DEBUG, "Chunks for world render prep: %zu", len);
  for (size_t i = 0; i < len; i++) {
    world_prepare_chunk_rendering_update_nearby(world, &world->chunks[i]);
    TraceLog(LOG_DEBUG, "Chunk at pos: %d, %d prepared", world->chunks[i].chunk_pos.x, world->chunks[i].chunk_pos.y);
  }
}

static bool background_tex_loaded = false;
static VariantTexture background_var_tex = {0};

void world_render_layer(World *world, TileLayer layer) {
  if (!background_tex_loaded) {
    cw_Texture tex = tex_by_handle(&CLIENT_GAME.asset_manager, TEX_DIRT);
    var_tex_load(&background_var_tex, &tex, &CLIENT_GAME.asset_manager);
    background_tex_loaded = true;
  }

  f32 seed_offset = world->seed * 37.77f;

  size_t len = array_len(world->chunks);
  for (size_t i = 0; i < len; i++) {
    Vec2i chunk_pos = world->chunk_lookup.chunks_positions[i];
    i32 chunk_x = chunk_pos.x * CHUNK_SIZE;
    i32 chunk_y = chunk_pos.y * CHUNK_SIZE;
    Chunk *chunk = &world->chunks[i];
    if (layer == TILE_LAYER_GROUND) {
      for (i32 y = 0; y < CHUNK_SIZE; y++) {
        for (i32 x = 0; x < CHUNK_SIZE; x++) {
          i32 world_x = chunk_x + x;
          i32 world_y = chunk_y + y;
          // TODO: Only render background tiles where ground layer tiles show background
          AssetId id = var_tex_for_pos(&background_var_tex, world_x, world_y, seed_offset);
          cw_Texture tex = tex_by_id(&CLIENT_GAME.asset_manager, id);

          DrawTexture(tex.texture, world_x * TILE_SIZE, world_y * TILE_SIZE, WHITE);

          TileInstance *tile = &chunk->tiles[y][x][layer];
          if (tile != NULL && tile->id == TILE_WATER) {
            TileProperties tile_props = CLIENT_GAME.game.registries.tiles[tile->id];
            DrawRectangle(world_x * TILE_SIZE, world_y * TILE_SIZE, 16, 16, tile_props.tile_color);
          }
        }
      }
    }

    for (i32 y = 0; y < CHUNK_SIZE; y++) {
      for (i32 x = 0; x < CHUNK_SIZE; x++) {
        i32 world_x = chunk_x + x;
        i32 world_y = chunk_y + y;
        TileInstance *tile = &chunk->tiles[y][x][layer];
        tile_render(tile, world_x * TILE_SIZE, world_y * TILE_SIZE, false);
      }
    }
  }
}

void world_render_layer_top_split(World *world, Rectangle player_box, bool draw_before_player) {
  bool correct_tile = false;
  float player_feet_y = player_box.y + player_box.height;

  size_t len = array_len(world->chunks);
  for (int i = 0; i < len; i++) {
    Chunk *chunk = &world->chunks[i];
    Vec2i chunk_pos = world->chunk_lookup.chunks_positions[i];
    int chunk_x = chunk_pos.x * CHUNK_SIZE;
    int chunk_y = chunk_pos.y * CHUNK_SIZE;

    for (int y = 0; y < CHUNK_SIZE; y++) {
      int world_y = chunk_y + y;

      for (int x = 0; x < CHUNK_SIZE; x++) {
        int world_x = chunk_x + x;

        TileInstance *tile = &chunk->tiles[y][x][TILE_LAYER_TOP];
        float tile_screen_y = (world_y + 0) * TILE_SIZE;

        bool should_draw = (tile_screen_y <= player_feet_y && draw_before_player) || (tile_screen_y > player_feet_y && !draw_before_player);

        if (should_draw && tile->id != TILE_EMPTY) {
          tile_render(tile, world_x * TILE_SIZE, world_y * TILE_SIZE, false);
        }
      }
    }
  }
}

void world_on_reload(ClientGame *client) {
  if (client->world != NULL) {
    world_prepare_rendering(client->world);
  }
}

void world_load(World *world, const DataMap *data, DataContext ctx) {
  SpaceId world_id = data_map_get(data, "world_id").var.data_int;
  world->space_id = world_id;
  DataList chunks_list = data_map_get(data, "chunks").var.data_list;
  size_t chunks = chunks_list.len;
  for (size_t i = 0; i < chunks; i++) {
    DataMap data_map = data_list_get(&chunks_list, i).var.data_map;
    Chunk chunk = {.space_id = world->space_id};
    chunk_load(&chunk, &data_map);
    world_add_chunk(world, chunk.chunk_pos, chunk);
  }
  log_debug("Total loaded chunks: %zu", chunks);
}

void world_save(const World *world, DataMap *data, DataContext ctx) {
  DataList chunks_list = data_list_new(WORLD_LOADED_CHUNKS, &HEAP_ALLOCATOR);
  size_t len = array_len(world->chunks);
  for (size_t i = 0; i < len; i++) {
    DataMap map = data_map_new(len, &HEAP_ALLOCATOR);
    const Chunk *chunk = &world->chunks[i];
    chunk_save(chunk, &map);
    data_list_add(&chunks_list, data_map(map));
  }
  data_map_insert(data, "world_id", data_int(world->space_id));
  data_map_insert(data, "chunks", data_list(chunks_list));
  log_debug("Total saved chunks: %zu", array_len(world->chunks));
}
