#pragma once

#include "data.h"
#include <raylib.h>

#define TILES_AMOUNT 3

typedef enum {
  TILE_EMPTY,
  TILE_DIRT,
  TILE_GRASS,
  TILE_STONE,
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
} TileInstance;

TileInstance tile_new(TileType *type, int x, int y);

void tile_render(TileInstance *tile);

void tile_right_click(TileInstance *tile);

void tile_tick(TileInstance *tile);

void tile_load(TileInstance *tile);

void tile_save(TileInstance *tile);

