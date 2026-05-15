#pragma once

#include "lilc/numbers.h"
#include "../assets.h"
#include "../tile/tile_props.h"
#include "items.h"

typedef enum {
  TILE_EMPTY,
  TILE_DIRT,
  TILE_GRASS,
  TILE_STONE,
  TILE_WATER,
  TILE_WORKSTATION,
  TILE_OVEN,
  TILE_TREE,
  TILE_TREE_STUMP,
  TILE_CHEST,
  TILE_DUNGEON_FLOOR,
  TILE_DUNGEON_PORTAL,

  _amount_tile_ids,
} TileId;

typedef enum {
  TILE_LAYER_GROUND,
  TILE_LAYER_TOP,
} TileLayer;

typedef struct {
  TileId surrounding_tiles[8];
} TileTextureData;

typedef struct {
  TileId id;
  char *id_literal;
  char *name;
  TileLayer layer;

  AssetId texture;
  bool has_texture;
  
  Dimensionsf tile_dimensions;
  ItemType *tile_item;
  TileProperties tile_props;

  // TEXTURES
  TileTextureProperties texture_props;
  i32 variant_index;
} TileType;
