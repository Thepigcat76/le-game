#pragma once

#include "assets.h"
#include "registry.h"

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

typedef struct {
  // Index for this is TileId, total amount is _amount_tile_ids
  VariantTexture *tile_variant_textures;
} TileTextureManager;

/* ANIMATION MANAGER */

void animation_manager_load(AnimationManager *anim_manager, const AssetManager *asset_manager);

void animation_manager_unload(AnimationManager *anim_manager, const AssetManager *asset_manager);

void animation_manager_tick(AnimationManager *anim_manager, const AssetManager *asset_manager);

i32 cw_tex_cur_frame(const cw_Texture *texture);

i32 cw_tex_frame_height(const cw_Texture *texture);

/* TILE TEXTURE MANAGER */

void tile_tex_manager_load(TileTextureManager *tile_tex_manager, const RegistryManager *registries, AssetManager *asset_manager);

void tile_tex_manager_unload(TileTextureManager *tile_tex_manager, const RegistryManager *registries, const AssetManager *asset_manager);

cw_Texture tile_tex(TileId id, TilePos tile_pos, f32 seed, const TileTextureManager *tile_texs, const RegistryManager *registries, const AssetManager *asset_manager);
