#pragma once

#include "lilc/numbers.h"
#include "../assets.h"
#include "../tile/tile_props.h"
#include "../tile/adv_tile.h"
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

  char *name;
  char *ident;
  
  TileLayer layer;

  // Visual
  AssetId texture;
  bool has_texture;
  
  // Property stuff
  Dimensionsf tile_dimensions;
  // ItemId, use -1 if no item is valid
  i32 tile_item;
  bool disable_collisions;
  int break_time;
  Color tile_color;

  // TEXTURES
  bool uses_tileset;
} TileProperties;

typedef struct {
  TileId id;
  Dimensionsf box;

  AdvTileInstance *adv_tile_instance;

  // TEXTURES
  TileTextureData texture_data;
  Rectf cur_sprite_box;
  AssetId variant_texture;
  i32 animation_frame;
} TileInstance;

extern TileInstance TILE_INST_EMPTY;

void tile_init(TileInstance *inst, TileId id);
