#include "../include/world.h"
#include "../include/being.h"
#include "../include/game.h"
#include "../include/item.h"
#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

Texture2D particle_texture;

World world_new_no_chunks() {
  particle_texture = LoadTexture("res/assets/particle.png");
  return (World){
      .chunks = NULL,
      .chunks_amount = 0,
      .initialized = false,
  };
}

World world_new() {
  particle_texture = LoadTexture("res/assets/particle.png");
  return (World){
      .chunks = malloc(WORLD_LOADED_CHUNKS * sizeof(Chunk)),
      .chunks_amount = 0,
      .initialized = false,
  };
}

void world_initialize(World *world) {
  world->initialized = true;
  world_prepare_rendering(world);
}

void world_add_chunk(World *world, ChunkPos pos, Chunk chunk) {
  world->chunks[world->chunks_amount] = chunk;
  world->chunk_lookup.indices[world->chunks_amount] = world->chunks_amount;
  world->chunk_lookup.chunks_positions[world->chunks_amount] = pos;
  world->chunks_amount++;
}

ssize_t world_chunk_index_by_pos(const World *world, ChunkPos pos) {
  for (size_t i = 0; i < world->chunks_amount; i++) {
    if (vec2_eq(&world->chunk_lookup.chunks_positions[i], &pos)) {
      return world->chunk_lookup.indices[i];
    }
  }
  return -1;
}

bool world_has_chunk_at(const World *world, Vec2i chunk_pos) {
  ssize_t index = world_chunk_index_by_pos(world, chunk_pos);
  return index != -1;
}

void world_gen_chunk_at(World *world, Vec2i chunk_pos) {
  Chunk chunk;

  chunk_gen(&chunk, chunk_pos, world->seed);

  world_add_chunk(world, chunk_pos, chunk);
}

TileInstance *world_ground_tile_at(World *world, TilePos tile_pos) {
  return world_tile_at(world, tile_pos, TILE_LAYER_GROUND);
}

TileInstance *world_highest_tile_at(World *world, TilePos tile_pos) {
  for (int l = TILE_LAYERS_AMOUNT - 1; l >= 0; l--) {
    TileInstance *tile = world_tile_at(world, tile_pos, l);
    if (tile->type.id != TILE_EMPTY || l == 0) {
      return tile;
    }
  }
  return &TILE_INSTANCE_EMPTY;
}

TileInstance *world_tile_at(World *world, TilePos tile_pos, TileLayer layer) {
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

void world_gen(World *world) { world_gen_chunk_at(world, vec2i(0, 0)); }

bool world_set_tile(World *world, TilePos tile_pos, TileInstance tile) {
  return world_set_tile_on_layer(world, tile_pos, tile, tile.type.layer);
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
  TileLayer layer = tile.type.layer;
  for (int l = layer - 1; l >= 0; l--) {
    if (world_tile_at(world, tile_pos, l)->type.id == TILE_EMPTY)
      return false;
    if (l == 0)
      break;
  }

  return world_set_tile_on_layer(world, tile_pos, tile, layer);
}

bool world_remove_tile(World *world, TilePos tile_pos) {
  TileInstance empty_instance = TILE_INSTANCE_EMPTY;
  TileInstance *tile = world_highest_tile_at(world, tile_pos);
  Color color = tile->type.tile_color;
  ItemType *item_type = tile->type.tile_item;
  if (world_set_tile_on_layer(world, tile_pos, empty_instance, tile->type.layer)) {
    if (item_type != NULL) {
      world_add_being(world,
                      being_item_new((ItemInstance){.type = *item_type},
                                     (tile_pos.x * TILE_SIZE) + GetRandomValue(-7, 9),
                                     (tile_pos.y * TILE_SIZE) + GetRandomValue(-7, 9)));
    }

    for (int i = 0; i < 5; i++) {
      ParticleInstance *particle =
          game_emit_particle(&GAME, tile_pos.x * TILE_SIZE + GetRandomValue(-9, 14),
                             tile_pos.y * TILE_SIZE + GetRandomValue(-9, 14), PARTICLE_TILE_BREAK,
                             (ParticleInstanceEx){.type = PARTICLE_INSTANCE_TILE_BREAK,
                                                  .var = {.tile_break = {.texture = particle_texture, .tint = color}}});
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
      if (tile->type.id != TILE_EMPTY) {
        world_set_tile_texture_data(world, tile, pos.x, pos.y);
      }
    }
  }

  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      TilePos pos = vec2i((chunk->chunk_pos.x * CHUNK_SIZE) + x, (chunk->chunk_pos.y * CHUNK_SIZE) + y);
      TileInstance *tile = world_ground_tile_at(world, vec2i(pos.x, pos.y));
      if (tile->type.id != TILE_EMPTY) {
        tile_calc_sprite_box(tile);
      }
    }
  }
}

void world_set_tile_texture_data(World *world, TileInstance *tile, int x, int y) {
  TileTextureData *texture_data = &tile->texture_data;
  texture_data->surrounding_tiles[0] = world_ground_tile_at(world, vec2i(x - 1, y - 1))->type.id;
  texture_data->surrounding_tiles[1] = world_ground_tile_at(world, vec2i(x, y - 1))->type.id;
  texture_data->surrounding_tiles[2] = world_ground_tile_at(world, vec2i(x + 1, y - 1))->type.id;
  texture_data->surrounding_tiles[3] = world_ground_tile_at(world, vec2i(x - 1, y))->type.id;
  texture_data->surrounding_tiles[4] = world_ground_tile_at(world, vec2i(x + 1, y))->type.id;
  texture_data->surrounding_tiles[5] = world_ground_tile_at(world, vec2i(x - 1, y + 1))->type.id;
  texture_data->surrounding_tiles[6] = world_ground_tile_at(world, vec2i(x, y + 1))->type.id;
  texture_data->surrounding_tiles[7] = world_ground_tile_at(world, vec2i(x + 1, y + 1))->type.id;
}

void world_prepare_rendering(World *world) {
  for (int i = 0; i < world->chunks_amount; i++) {
    world_prepare_chunk_rendering(world, &world->chunks[i]);
  }
}

void world_render_layer(World *world, TileLayer layer) {
  for (int i = 0; i < world->chunks_amount; i++) {
    Vec2i chunk_pos = world->chunk_lookup.chunks_positions[i];
    int chunk_x = chunk_pos.x * CHUNK_SIZE;
    int chunk_y = chunk_pos.y * CHUNK_SIZE;
    Chunk *chunk = &world->chunks[i];
    if (layer == TILE_LAYER_GROUND) {
      for (int y = chunk_y; y < chunk_y + CHUNK_SIZE; y++) {
        for (int x = chunk_x; x < chunk_x + CHUNK_SIZE; x++) {
          DrawTexture(tile_variants_by_index(chunk->variant_index, 0,
                                             0)[chunk->background_texture_variants[y - chunk_y][x - chunk_x]],
                      x * TILE_SIZE, y * TILE_SIZE, WHITE);
        }
      }
    }

    for (int y = 0; y < CHUNK_SIZE; y++) {
      for (int x = 0; x < CHUNK_SIZE; x++) {
        TileInstance *tile = &chunk->tiles[y][x][layer];
        tile_render(tile, (chunk_x + x) * TILE_SIZE, (chunk_y + y) * TILE_SIZE);
      }
    }
  }
}

void world_render_layer_top_split(World *world, void *_player, bool draw_before_player) {
  Player *player = (Player *)_player;
  float player_feet_y = player->box.y + player->box.height;

  for (int i = 0; i < world->chunks_amount; i++) {
    Chunk *chunk = &world->chunks[i];
    Vec2i chunk_pos = world->chunk_lookup.chunks_positions[i];
    int chunk_x = chunk_pos.x * CHUNK_SIZE;
    int chunk_y = chunk_pos.y * CHUNK_SIZE;

    for (int y = 0; y < CHUNK_SIZE; y++) {
      int world_y = chunk_y + y;

      for (int x = 0; x < CHUNK_SIZE; x++) {
        int world_x = chunk_x + x;

        TileInstance *tile = &chunk->tiles[y][x][TILE_LAYER_TOP];
        float tile_screen_y = (world_y + 1) * TILE_SIZE;

        bool should_draw = (tile_screen_y <= player_feet_y && draw_before_player) ||
            (tile_screen_y > player_feet_y && !draw_before_player);

        if (should_draw) {
          tile_render(tile, world_x * TILE_SIZE, world_y * TILE_SIZE);
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

void load_world(World *world, const DataMap *data) {
  DataList chunks_list = data_map_get(data, "chunks").var.data_list;
  size_t chunks = chunks_list.len;
  for (int i = 0; i < chunks; i++) {
    DataMap data_map = data_list_get(&chunks_list, i).var.data_map;
    Chunk chunk;
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

void save_world(const World *world, DataMap *data) {
  DataList chunks_list = data_list_new(WORLD_LOADED_CHUNKS);
  for (int i = 0; i < world->chunks_amount; i++) {
    DataMap map = data_map_new(8);
    const Chunk *chunk = &world->chunks[i];
    chunk_save(chunk, &map);
    char key[2] = {i, '\0'};
    data_list_add(&chunks_list, data_map(map));
  }
  data_map_insert(data, "chunks", data_list(chunks_list));
  TraceLog(LOG_DEBUG, "Total saved chunks: %u", world->chunks_amount);
  world_save_beings(world, data);
  TraceLog(LOG_DEBUG, "Total saved beings: %d", world->beings_amount);
}
