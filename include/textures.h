#pragma once

#include "assets.h"
#include "registry.h"
#include "shared.h"
#include "tile.h"

/* ANIMATION MANAGER */

typedef struct {
  AssetId texture;
  u32 width;
  u32 height;
  u32 frame_height;
  size_t frames;

  i32 cur_frame;
  f32 frame_timer;
} AnimatedTexture;

typedef struct {
  // Index for this is AssetId, total amount is array_len(asset_manager->textures)
  AnimatedTexture *animated_textures;
} AnimationManager;

/* VARIANT TEXTURES */

typedef struct {
  // Links to the base asset. In the case of dirt this would be the id of 'TEX_DIRT',
  // which in turn links to the AssetIds of its variants
  AssetId id;
  enum {
    TEX_VAR_NONE,
    TEX_VAR_SINGLE,
    TEX_VAR_CONNECTED,
  } kind;
  AssetId *variants;
} VariantTexture;

/* TILE TEXTURE MANAGER */

typedef struct {
  i32 predicates[8];
  i32 predicates_amount;
  i32 ignored_tiles[8];
  i32 ignored_tiles_amount;
  Vec2i sprite_pos;
} Connection;

typedef struct {
  // Index for this is TileId, total amount is _amount_tile_ids
  VariantTexture *tile_variant_textures;

  // Connected textures
  Vec2i default_sprite_pos;
  u32 default_resolution;
  Connection *connections;
} TileTextureManager;

/* GENERAL */

void tex_draw0(cw_Texture *tex, Vec2i pos, Color tint);

void tex_draw1(cw_Texture *tex, Vec2i pos, f32 scale, Color tint);

void tex_draw2(cw_Texture *tex, Vec2i dest, Rectf src, f32 rotation, f32 scale, Color tint);

void tex_draw3(cw_Texture *tex, Rectf dest, Rectf src, Vec2f origin, f32 rotation, Color tint);

/* VARIANT TEXTURES */

void var_tex_load(VariantTexture *var_tex, const cw_Texture *tex, AssetManager *assets);

AssetId var_tex_for_pos(VariantTexture *var_tex, i32 x, i32 y, f32 seed_offset);

/* ANIMATION MANAGER */

void animation_manager_load(AnimationManager *anim_manager, const AssetManager *asset_manager);

void animation_manager_unload(AnimationManager *anim_manager, const AssetManager *asset_manager);

void animation_manager_tick(AnimationManager *anim_manager, const AssetManager *asset_manager);

i32 cw_tex_cur_frame(const cw_Texture *texture);

i32 cw_tex_frame_height(const cw_Texture *texture);

/* TILE TEXTURE MANAGER */

void tile_tex_manager_load(TileTextureManager *tile_tex_manager, const RegistryManager *registries, AssetManager *asset_manager);

void tile_tex_manager_unload(TileTextureManager *tile_tex_manager, const RegistryManager *registries, const AssetManager *asset_manager);

cw_Texture tile_tex(TileId id, TilePos tile_pos, f32 seed, const TileTextureManager *tile_texs, const RegistryManager *registries,
                    const AssetManager *asset_manager);

void tile_calc_sprite_box(TileInstance *tile);
