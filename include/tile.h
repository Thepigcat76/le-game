#pragma once

#include "data.h"
#include <raylib.h>

#define TILES_AMOUNT 2

typedef enum {
  TILE_GRASS,
  TILE_STONE,
} TileId;

typedef struct {
  TileId id;
  bool has_texture;
  Texture2D texture;
  bool is_solid;

  // ADVANCED TILE PROPERTIES
  bool is_ticking;
  bool stores_custom_data;
} TileType;

extern TileType TILES[];

void tile_type_init(TileId id);

char *tile_type_to_string(TileType *type);

typedef struct {
  TileType type;
  Rectangle box;
  
  // ADVANCED
  Data custom_data;
} TileInstance;

void tile_right_click(TileInstance *tile);

void tile_tick(TileInstance *tile);

void tile_load(TileInstance *tile);

void tile_save(TileInstance *tile);

