#pragma once

#include "assets.h"
#include "data.h"
#include "game_feature.h"
#include "item.h"
#include "raylib.h"
#include "shared.h"
#include "textures.h"
#include "tile/tile_category.h"
#include "tile/tile_props.h"
#include "tile/adv_tile.h"

#define DEFAULT_TILE_DIMENSIONS dimf(16, 16)

extern AdvTileInstance *ADV_TILES;

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
} TileId;

typedef struct {
  TileId surrounding_tiles[8];
} TileTextureData;

typedef enum {
  TILE_LAYER_GROUND,
  TILE_LAYER_TOP,
} TileLayer;

TileLayer tile_layer_from_str(const char *layer_literal);

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

extern TileType *TILES;
extern size_t TILES_AMOUNT;

//extern AdvTexture ERR_TEXTURE;

void tile_type_init(TileType *type);

void tile_types_init();

char *tile_type_to_string(const TileType *type);

void tile_type_debug_print(const TileType *type, char *buf);

typedef struct {
  const TileType *type;
  Dimensionsf box;

  AdvTileInstance *adv_tile_instance;

  // TEXTURES
  TileTextureData texture_data;
  Rectf cur_sprite_box;
  //AdvTexture variant_texture;
  i32 animation_frame;
} TileInstance;

extern TileInstance TILE_INSTANCE_EMPTY;

TileInstance tile_new(const TileType *type);

void tile_instance_debug(const TileInstance *tile, char *buf);

Rectf tile_collision_box_at(const TileInstance *tile, i32 x, i32 y);

Dimensionsf tile_collision_dimensions_at(const TileInstance *tile);

Vec2f tile_collision_offset_at(const TileInstance *tile);

void tile_render(TileInstance *tile, i32 x, i32 y, bool dbg);

void tile_render_scaled(TileInstance *tile, i32 x, i32 y, float scale);

void tile_right_click(TileInstance *tile);

TileInstance tile_break_remainder(const TileInstance *tile, TilePos pos);

void tile_tick(TileInstance *tile);

// CONNECTED TEXTURES

Vec2i tile_default_sprite_pos();

i32 tile_default_sprite_resolution();

void tile_calc_sprite_box(TileInstance *tile);

// TEXTURE VARIANTS

// X and Y params are only nessecary in tile sheets, otherwise you can just pass
// in 0
cw_Texture *tile_variants_for_tile(const TileType *type, i32 x, i32 y);

i32 tile_variants_index_for_name(const char *texture_name, i32 x, i32 y);

i32 tile_variants_amount_for_tile(const TileType *type, i32 x, i32 y);

cw_Texture *tile_variants_by_index(i32 i, i32 x, i32 y);

i32 tile_variants_amount_by_index(i32 index, i32 x, i32 y);

void tile_variants_free();

// TILE CATEGORIES

typedef struct {
  TileId tiles[MAX_TILE_TYPES];
  TileIdCategories tile_categories[MAX_TILE_TYPES];
  size_t tiles_amount;
} TileCategoryLookup;

void tile_categories_setup(TileCategoryLookup *lookup);

TileIdCategories tile_categories(const TileCategoryLookup *lookup, const TileType *tile);
