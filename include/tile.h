#pragma once

#include "data.h"
#include "item.h"
#include "raylib.h"
#include "shared.h"
#include "textures.h"
#include "tile_category.h"

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
  AdvTexture texture;
  bool is_solid;
  TileLayer layer;
  Color tile_color;
  int tile_width;
  int tile_height;
  int break_time;
  ItemType *tile_item;

  // ADVANCED TILE PROPERTIES
  bool is_ticking;
  bool stores_custom_data;
  bool uses_tileset;
  int variant_index;
  bool has_animation;
} TileType;

extern TileType TILES[MAX_TILE_TYPES];
extern size_t TILES_AMOUNT;

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
  AdvTexture variant_texture;
  int animation_frame;
} TileInstance;

extern TileInstance TILE_INSTANCE_EMPTY;

TileInstance tile_new(TileType type);

Rectf tile_collision_box_at(const TileInstance *tile, int x, int y);

Dimensionsf tile_collision_dimensions_at(const TileInstance *tile);

Vec2f tile_collision_offset_at(const TileInstance *tile);

void tile_render(TileInstance *tile, int x, int y);

void tile_render_scaled(TileInstance *tile, int x, int y, float scale);

void tile_right_click(TileInstance *tile);

TileInstance tile_break_remainder(const TileInstance *tile, TilePos pos);

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
AdvTexture *tile_variants_for_tile(const TileType *type, int x, int y);

int tile_variants_index_for_name(const char *texture_name, int x, int y);

int tile_variants_amount_for_tile(const TileType *type, int x, int y);

AdvTexture *tile_variants_by_index(int i, int x, int y);

int tile_variants_amount_by_index(int index, int x, int y);

void tile_variants_free();

// TILE CATEGORIES

typedef struct {
  TileId tiles[MAX_TILE_TYPES];
  TileIdCategories tile_categories[MAX_TILE_TYPES];
  size_t tiles_amount;
} TileCategoryLookup;

void tile_categories_init(void);

TileIdCategories tile_categories(const TileType *tile);
