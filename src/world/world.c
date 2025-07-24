#include "../../include/world.h"
#include "../../include/array.h"
#include "../../include/being.h"
#include "../../include/item.h"
#include "../../include/net/client.h"
#include "../../include/particle.h"
#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

WorldType *WORLD_TYPES;

Texture2D particle_texture;

World world_new_no_chunks(void) {
  particle_texture = LoadTexture("res/assets/particle.png");
  return (World){
      .chunks = NULL,
      .initialized = false,
      .type = NULL,
  };
}

World world_new(const WorldType *world_type, float seed) {
  particle_texture = LoadTexture("res/assets/particle.png");
  return (World){
      .chunks = array_new_capacity(Chunk, WORLD_LOADED_CHUNKS, &HEAP_ALLOCATOR),
      .chunk_lookup = {.chunks_positions = array_new_capacity(ChunkPos, WORLD_LOADED_CHUNKS, &HEAP_ALLOCATOR),
                       .indices = array_new_capacity(size_t, WORLD_LOADED_CHUNKS, &HEAP_ALLOCATOR)},
      .initialized = false,
      .type = world_type,
      .seed = seed,
  };
}

void world_types_init(void) {
  WORLD_TYPES = array_new_capacity(WorldType, 16, &HEAP_ALLOCATOR);
  WORLD_TYPES[WORLD_BASE] = (WorldType){.id = WORLD_BASE};
  WORLD_TYPES[WORLD_DUNGEON_TEST] = (WorldType){.id = WORLD_DUNGEON_TEST};
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
  for (int i = 0; i < len; i++) {
    printf("Chunk %d, %d - %zu\n", lookup->chunks_positions[i].x, lookup->chunks_positions[i].y, lookup->indices[i]);
  }
}

void world_gen_chunk_at(World *world, ChunkPos chunk_pos) {
  Chunk chunk = {.world_type = world->type};

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
    if (tile->type->id != TILE_EMPTY || l == 0) {
      return tile;
    }
  }
  return &TILE_INSTANCE_EMPTY;
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

  return &TILE_INSTANCE_EMPTY;
}

static const Vec2i RENDER_CHUNK_OFFSETS[4] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

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

  world_initialize(world);
}

bool world_set_tile(World *world, TilePos tile_pos, TileInstance tile) {
  return world_set_tile_on_layer(world, tile_pos, tile, tile.type->layer);
}

bool world_set_tile_on_layer(World *world, TilePos tile_pos, TileInstance tile, TileLayer layer) {
  int chunk_tile_x = floor_mod(tile_pos.x, CHUNK_SIZE);
  int chunk_tile_y = floor_mod(tile_pos.y, CHUNK_SIZE);
  int chunk_x = floor_div((tile_pos.x - chunk_tile_x), CHUNK_SIZE);
  int chunk_y = floor_div((tile_pos.y - chunk_tile_y), CHUNK_SIZE);
  ChunkPos chunk_pos = vec2i(chunk_x, chunk_y);
  printf("set tile\n");
  if (world_has_chunk_at(world, chunk_pos)) {
    printf("has chunk\n");
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
  TileLayer layer = tile.type->layer;
  for (int l = layer; l >= 0; l--) {
    if (l == 0) {
      break;
    }

    if (world_tile_at(world, tile_pos, l)->type->id == TILE_EMPTY) {
      return false;
    }
  }

  return world_set_tile_on_layer(world, tile_pos, tile, layer);
}

bool world_remove_tile(World *world, TilePos tile_pos) {
  TileInstance empty_instance = TILE_INSTANCE_EMPTY;
  TileInstance *tile = world_highest_tile_at(world, tile_pos);
  Color color = tile->type->tile_props.tile_color;
  ItemType *item_type = tile->type->tile_item;
  if (world_set_tile_on_layer(world, tile_pos, empty_instance, tile->type->layer)) {
    if (item_type != NULL) {
      world_add_being(world,
                      being_item_new((ItemInstance){.type = *item_type}, (tile_pos.x * TILE_SIZE) + GetRandomValue(-7, 9),
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

void world_add_being(World *world, BeingInstance being) {
  if (world->beings_amount < MAX_WORLD_BEINGS_AMOUNT) {
    world->beings[world->beings_amount] = being;
    world->beings[world->beings_amount].being_instance_id = world->beings_amount;
    world->beings_amount++;
  }
}

void world_remove_being(World *world, BeingInstance *being) {
  for (int i = 0; i < world->beings_amount; i++) {
    if (world->beings[i].being_instance_id == being->being_instance_id) {
      // Shift everything after i left by one
      for (int j = i; j < world->beings_amount - 1; j++) {
        world->beings[j] = world->beings[j + 1];
      }
      world->beings_amount--;
      return;
    }
  }
}

void world_prepare_chunk_rendering(World *world, Chunk *chunk) {
  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      int chunk_x = chunk->chunk_pos.x * CHUNK_SIZE;
      int chunk_y = chunk->chunk_pos.y * CHUNK_SIZE;
      TilePos pos = vec2i(chunk_x + x, chunk_y + y);
      TileInstance *tile = world_ground_tile_at(world, vec2i(pos.x, pos.y));
      if (tile->type->id != TILE_EMPTY) {
        world_set_tile_texture_data(world, tile, pos.x, pos.y);
      }
    }
  }

  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      TilePos pos = vec2i((chunk->chunk_pos.x * CHUNK_SIZE) + x, (chunk->chunk_pos.y * CHUNK_SIZE) + y);
      TileInstance *tile = world_ground_tile_at(world, vec2i(pos.x, pos.y));
      if (tile->type->id != TILE_EMPTY) {
        tile_calc_sprite_box(tile);
      }
    }
  }
}

void world_set_tile_texture_data(World *world, TileInstance *tile, int x, int y) {
  TileTextureData *texture_data = &tile->texture_data;
  texture_data->surrounding_tiles[0] = world_ground_tile_at(world, vec2i(x - 1, y - 1))->type->id;
  texture_data->surrounding_tiles[1] = world_ground_tile_at(world, vec2i(x, y - 1))->type->id;
  texture_data->surrounding_tiles[2] = world_ground_tile_at(world, vec2i(x + 1, y - 1))->type->id;
  texture_data->surrounding_tiles[3] = world_ground_tile_at(world, vec2i(x - 1, y))->type->id;
  texture_data->surrounding_tiles[4] = world_ground_tile_at(world, vec2i(x + 1, y))->type->id;
  texture_data->surrounding_tiles[5] = world_ground_tile_at(world, vec2i(x - 1, y + 1))->type->id;
  texture_data->surrounding_tiles[6] = world_ground_tile_at(world, vec2i(x, y + 1))->type->id;
  texture_data->surrounding_tiles[7] = world_ground_tile_at(world, vec2i(x + 1, y + 1))->type->id;
}

void world_prepare_rendering(World *world) {
  size_t len = array_len(world->chunks);
  TraceLog(LOG_DEBUG, "Chunks for world render prep: %zu", len);
  for (int i = 0; i < len; i++) {
    world_prepare_chunk_rendering_update_nearby(world, &world->chunks[i]);
    TraceLog(LOG_DEBUG, "Chunk at pos: %d, %d prepared", world->chunks[i].chunk_pos.x, world->chunks[i].chunk_pos.y);
  }
}

void world_render_layer(World *world, TileLayer layer) {
  size_t len = array_len(world->chunks);
  for (int i = 0; i < len; i++) {
    Vec2i chunk_pos = world->chunk_lookup.chunks_positions[i];
    int chunk_x = chunk_pos.x * CHUNK_SIZE;
    int chunk_y = chunk_pos.y * CHUNK_SIZE;
    Chunk *chunk = &world->chunks[i];
    if (layer == TILE_LAYER_GROUND) {
      for (int y = chunk_y; y < chunk_y + CHUNK_SIZE; y++) {
        for (int x = chunk_x; x < chunk_x + CHUNK_SIZE; x++) {
          DrawTexture(adv_texture_to_texture(&tile_variants_by_index(chunk->variant_index, 0,
                                                                     0)[chunk->background_texture_variants[y - chunk_y][x - chunk_x]]),
                      x * TILE_SIZE, y * TILE_SIZE, WHITE);
        }
      }
    }

    for (int y = 0; y < CHUNK_SIZE; y++) {
      for (int x = 0; x < CHUNK_SIZE; x++) {
        int world_x = chunk_x + x;
        int world_y = chunk_y + y;
        TileInstance *tile = &chunk->tiles[y][x][layer];
        tile_render(tile, world_x * TILE_SIZE, world_y * TILE_SIZE, false);
      }
    }
  }
}

void world_render_layer_top_split(World *world, void *_player, bool draw_before_player) {
  bool correct_tile = false;
  Player *player = (Player *)_player;
  float player_feet_y = player->box.y + player->box.height;

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

        if (should_draw && tile->type->id != TILE_EMPTY) {
          tile_render(tile, world_x * TILE_SIZE, world_y * TILE_SIZE, false);
        }
      }
    }
  }
}

static void world_load_beings(World *world, const DataMap *data) {
  DataList list = data_map_get_or_default(data, "beings", data_list(data_list_new(0))).var.data_list;
  for (int i = 0; i < list.len; i++) {
    DataMap being_data = data_list_get(&list, i).var.data_map;
    BeingId id = data_map_get_or_default(&being_data, "id", data_int(BEING_NPC)).var.data_int;
    BeingInstance being_instance = being_new_default(id);
    being_load(&being_instance, &being_data);
    world_add_being(world, being_instance);
  }
}

void world_load(World *world, const DataMap *data) {
  WorldId world_id = data_map_get(data, "world_id").var.data_int;
  world->type = &WORLD_TYPES[world_id];
  DataList chunks_list = data_map_get(data, "chunks").var.data_list;
  size_t chunks = chunks_list.len;
  for (int i = 0; i < chunks; i++) {
    DataMap data_map = data_list_get(&chunks_list, i).var.data_map;
    Chunk chunk = {.world_type = world->type};
    chunk_load(&chunk, &data_map);
    world_add_chunk(world, chunk.chunk_pos, chunk);
  }
  TraceLog(LOG_DEBUG, "Total loaded chunks: %u", chunks);
  world_load_beings(world, data);
  TraceLog(LOG_DEBUG, "Total loaded beings: %d", world->beings_amount);
}

static void world_save_beings(const World *world, DataMap *data) {
  DataList list = data_list_new(world->beings_amount);
  for (int i = 0; i < world->beings_amount; i++) {
    const BeingInstance *instance = &world->beings[i];
    DataMap being_data = data_map_new(10);
    data_map_insert(&being_data, "id", data_int(instance->id));
    being_save(instance, &being_data);
    data_list_add(&list, data_map(being_data));
  }
  data_map_insert(data, "beings", data_list(list));
}

void world_save(const World *world, DataMap *data) {
  DataList chunks_list = data_list_new(WORLD_LOADED_CHUNKS);
  size_t len = array_len(world->chunks);
  for (int i = 0; i < len; i++) {
    DataMap map = data_map_new(8);
    const Chunk *chunk = &world->chunks[i];
    chunk_save(chunk, &map);
    data_list_add(&chunks_list, data_map(map));
  }
  data_map_insert(data, "world_id", data_int(world->type->id));
  data_map_insert(data, "chunks", data_list(chunks_list));
  TraceLog(LOG_DEBUG, "Total saved chunks: %zu", array_len(world->chunks));
  world_save_beings(world, data);
  TraceLog(LOG_DEBUG, "Total saved beings: %d", world->beings_amount);
}
