#pragma once

#include "data.h"
#include "item.h"
#include "shared.h"
#include <raylib.h>

typedef enum {
  TILE_EMPTY,
  TILE_DIRT,
  TILE_GRASS,
  TILE_STONE,
  TILE_WATER,
  TILE_WORKSTATION,
  TILE_OVEN,
  TILE_TREE,
} TileId;

typedef struct {
  TileId surrounding_tiles[8];
} TileTextureData;

typedef enum {
  TILE_LAYER_GROUND,
  TILE_LAYER_TOP,
} TileLayer;

typedef struct {
  TileId id;
  bool has_texture;
  char *texture_path;
  Texture2D texture;
  bool is_solid;
  TileLayer layer;
  Color tile_color;
  int tile_width;
  int tile_height;
  ItemType *tile_item;

  // ADVANCED TILE PROPERTIES
  bool is_ticking;
  bool stores_custom_data;
  bool uses_tileset;
  int variant_index;
  bool has_animation;
} TileType;

extern TileType TILES[TILE_TYPE_AMOUNT];

void tile_type_init(TileType *type);

void tile_types_init();

char *tile_type_to_string(const TileType *type);

typedef struct {
  TileType type;
  Dimensionsf box;

  // ADVANCED
  Data custom_data;
  TileTextureData texture_data;
  Rectangle cur_sprite_box;
  Texture2D variant_texture;
  int animation_frame;
} TileInstance;

extern TileInstance TILE_INSTANCE_EMPTY;

TileInstance tile_new(TileType type);

Rectf tile_collision_box_at(const TileInstance *tile, int x, int y);

void tile_render(TileInstance *tile, int x, int y);

void tile_render_scaled(TileInstance *tile, int x, int y, float scale);

void tile_right_click(TileInstance *tile);

void tile_tick(TileInstance *tile);

void tile_load(TileInstance *tile, const DataMap *data);

void tile_save(const TileInstance *tile, DataMap *data);

// CONNECTED TEXTURES

Vec2i tile_default_sprite_pos();

int tile_default_sprite_resolution();

void tile_calc_sprite_box(TileInstance *tile);

// TEXTURE VARIANTS

// X and Y params are only nessecary in tile sheets, otherwise you can just pass
// in 0
Texture2D *tile_variants_for_tile(const TileType *type, int x, int y);

int tile_variants_index_for_name(const char *texture_name, int x, int y);

int tile_variants_amount_for_tile(const TileType *type, int x, int y);

Texture2D *tile_variants_by_index(int i, int x, int y);

int tile_variants_amount_by_index(int index, int x, int y);

void tile_variants_free();
