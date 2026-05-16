#pragma once

#include "assets/shaders.h"
#include "assets/sounds.h"
#include "assets/textures.h"
#include "lilc/alloc.h"
#include "lilc/bump.h"
#include "lilc/numbers.h"
#include "shared.h"
#include <raylib.h>

#define ASSETS_DIR "res/assets/"

#define TEXTURES_DIR "tex"
#define MUSIC_DIR "music"
#define SHADERS_DIR "shaders"
#define SOUNDS_DIR "sounds"

typedef struct {
  char **variant_paths;
  bool has_variants;
  struct {
    size_t frames;
    u32 frame_time;
    u32 frame_height;
  } animation;
  bool has_animation;
} TextureMetaInfo;

typedef struct {
  AssetId id;
  Texture2D texture;
  u32 width;
  u32 height;
  const char *path;
  
  TextureMetaInfo meta_info;
  bool has_meta_info;
} cw_Texture;

typedef struct {
  AssetId id;
  Shader shader;

  i32 shader_values_locs[64];
  size_t shader_values_locs_amount;

  // Base path, without file extension or _meta
  const char *base_path;
} cw_Shader;

typedef struct {
  AssetId id;
  Sound sound;
  const char *path;
} cw_Sound;

typedef struct {
  cw_Texture *textures;
  cw_Shader *shaders;
  cw_Sound *sounds;

  Bump asset_bump;
  Allocator asset_bump_allocator;
} AssetManager;

void assets_load(AssetManager *asset_manager);

void assets_unload(AssetManager *asset_manager);

// Getters

/* TEXTURES */

cw_Texture tex_by_id(const AssetManager *asset_manager, AssetId id);

cw_Texture tex_by_handle(const AssetManager *asset_manager, TextureHandle handle);

// Uses a texture path, which drops res/assets/tes/ and .png
cw_Texture tex_by_tex_path(const AssetManager *asset_manager, const char *tex_path);

/* SHADERS */

cw_Shader cw_shader_by_id(AssetManager *asset_manager, AssetId id);

cw_Shader cw_shader_by_handle(AssetManager *asset_manager, ShaderHandle handle);

cw_Shader cw_shader_by_shader_path(AssetManager *asset_manager, const char *shader_path);

Shader shader_by_id(AssetManager *asset_manager, AssetId id);

Shader shader_by_handle(AssetManager *asset_manager, ShaderHandle id);

/* SOUNDS */

cw_Sound cw_sound_by_id(AssetManager *asset_manager, AssetId id);

cw_Sound cw_sound_by_handle(AssetManager *asset_manager, SoundHandle handle);

cw_Sound cw_sound_by_sound_path(AssetManager *asset_manager, const char *sound_path);

Sound sound_by_id(AssetManager *asset_manager, AssetId id);

Sound sound_by_handle(AssetManager *asset_manager, SoundHandle id);

// Load asset files

i32 cw_texture_load(cw_Texture *texture, AssetManager *manager, FileEntry file_entry);

void cw_texture_unload(cw_Texture *texture);

i32 cw_shader_load(cw_Shader *shader, AssetManager *manager, FileEntry vs_file_entry, FileEntry fs_file_entry, FileEntry meta_file_entry);

void cw_shader_unload(cw_Shader *shader);

i32 cw_sound_load(cw_Sound *sound, AssetManager *manager, FileEntry file_entry);

void cw_sound_unload(cw_Sound *sound);

// Assign ids to handles

void texture_handles_assign_id(AssetManager *m);

void shader_handles_assign_id(AssetManager *m);

void sound_handles_assign_id(AssetManager *m);
