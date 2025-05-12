#pragma once

#include "data.h"
#include <raylib.h>

#define TILES_AMOUNT 4

typedef enum {
  TILE_EMPTY = 0,
  TILE_DIRT = 1,
  TILE_GRASS = 2,
  TILE_STONE = 3,
} TileId;

typedef struct {
  TileId surrounding_tiles[8];
} TileTextureData;

typedef struct {
  TileId id;
  bool has_texture;
  Texture2D texture;
  bool is_solid;

  // ADVANCED TILE PROPERTIES
  bool is_ticking;
  bool stores_custom_data;
  bool uses_tileset;
} TileType;

extern TileType TILES[];

void tile_types_init();

void tile_type_init(TileId id);

char *tile_type_to_string(TileType *type);

typedef struct {
  TileType type;
  Rectangle box;
  
  // ADVANCED
  Data custom_data;
  TileTextureData texture_data;
  Rectangle cur_sprite_box;
} TileInstance;

TileInstance tile_new(const TileType *type, int x, int y);

void tile_render(const TileInstance *tile);

void tile_right_click(TileInstance *tile);

void tile_tick(TileInstance *tile);

void tile_load(TileInstance *tile, const DataMap *data);

void tile_save(const TileInstance *tile, DataMap *data);

// CONNECTED TEXTURES

void tile_on_reload();

void tile_calc_sprite_box(TileInstance *tile);

void init_connected_info();
