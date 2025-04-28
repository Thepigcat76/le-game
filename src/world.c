#include "../include/world.h"
#include "../include/stb_perlin.h"
#include <raylib.h>
#include <stdio.h>

World world_new() { return (World){}; }

void world_gen(World *world) {
  Chunk chunk;

  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      float fx = x * 0.1;
      float fy = y * 0.1;
      int noise = (stb_perlin_noise3(fx, fy, 0.0f, 0, 0, 0) + 1) * 10;

      TileType type;
      if (noise > 5) {
        if (noise < 8) {
          type = TILES[TILE_DIRT];
        } else {
          type = TILES[TILE_GRASS];
        }
      } else {
        type = TILES[TILE_STONE];
      }
      chunk.tiles[y][x] = tile_new(&type, x * TILE_SIZE, y * TILE_SIZE);
    }
  }

  world->chunks[0][0] = chunk;
}

void world_prepare_rendering(World *world) {
  Chunk *chunk = &world->chunks[0][0];
  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      TileTextureData *texture_data = &chunk->tiles[y][x].texture_data;
      texture_data->surrounding_tiles[0] =
          y > 0 && x > 0 ? chunk->tiles[y - 1][x - 1].type.id : TILE_EMPTY;
      texture_data->surrounding_tiles[1] =
          y > 0 ? chunk->tiles[y - 1][x].type.id : TILE_EMPTY;
      texture_data->surrounding_tiles[2] =
          y > 0 && x < CHUNK_SIZE - 1 ? chunk->tiles[y - 1][x + 1].type.id
                                      : TILE_EMPTY;
      texture_data->surrounding_tiles[3] =
          x > 0 ? chunk->tiles[y][x - 1].type.id : TILE_EMPTY;
      texture_data->surrounding_tiles[4] =
          x < CHUNK_SIZE - 1 ? chunk->tiles[y][x + 1].type.id : TILE_EMPTY;
      texture_data->surrounding_tiles[5] =
          y < CHUNK_SIZE - 1 && x > 0 ? chunk->tiles[y + 1][x - 1].type.id
                                      : TILE_EMPTY;
      texture_data->surrounding_tiles[6] =
          y < CHUNK_SIZE - 1 ? chunk->tiles[y + 1][x].type.id : TILE_EMPTY;
      texture_data->surrounding_tiles[7] =
          y < CHUNK_SIZE - 1 && x < CHUNK_SIZE - 1
              ? chunk->tiles[y + 1][x + 1].type.id
              : TILE_EMPTY;
    }
  }

  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      tile_calc_sprite_box(&chunk->tiles[y][x]);
    }
  }
}

void world_render(World *world) {
  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      DrawTexture(TILES[TILE_DIRT].texture, x * TILE_SIZE, y * TILE_SIZE,
                  WHITE);
    }
  }
  Chunk *chunk = &world->chunks[0][0];
  for (int y = 0; y < CHUNK_SIZE; y++) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
      TileInstance *tile = &chunk->tiles[y][x];
      tile_render(tile);
    }
  }
}

void load_world(World *world, DataMap *data) {
  DataMap map = data_map_get(data, "chunk0,0").var.data_map;
  chunk_load(&world->chunks[0][0], &map);
}

// TODO: Dealloc... pretty much everything
void save_world(World *world, DataMap *data) {
  DataMap map = data_map_new(400);
  chunk_save(&world->chunks[0][0], &map);
  data_map_insert(data, "chunk0,0", data_map(map));
}
