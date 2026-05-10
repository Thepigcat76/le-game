#pragma once

#include "lilc/alloc.h"
#include "lilc/bump.h"
#include "lilc/numbers.h"
#include "shared.h"
#include "textures.h"
#include <raylib.h>

#define ASSETS_DIR "res/assets/"

#define TEXTURES_DIR "tex"
#define MUSIC_DIR "music"
#define SHADERS_DIR "shaders"
#define SOUNDS_DIR "sounds"

typedef struct {
  enum {
    TEXTURE_STATIC,
    TEXTURE_ANIMATED,
  } kind;
  union {
    Texture2D texture_static;
    struct {
      int animated_texture_id;
      Texture2D texture;
      int frames;
      int frame_time;
    } texture_animated;
  } var;
  AssetId id;
  int width;
  int height;
  const char *path;
} cw_Texture;

typedef struct {
  cw_Texture texture;
  int animated_texture_id;
  int cur_frame;
  float frame_timer;
} AnimatedTexture;

typedef struct {
  AssetId id;
  Shader shader;

  i32 shader_variable_locs[64];
} cw_Shader;

typedef struct {
  AssetId id;
  Sound sound;
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

cw_Texture cw_tex_by_id(AssetManager *asset_manager, AssetId id);

cw_Texture cw_tex_by_handle(AssetManager *asset_manager, TextureHandle handle);

Texture2D tex_by_id(AssetManager *asset_manager, AssetId id);

Texture2D tex_by_handle(AssetManager *asset_manager, TextureHandle handle);

// Load asset files

i32 cw_texture_load(cw_Texture *texture, AssetManager *manager, FileEntry file_entry);

// Assign ids to handles

void texture_handles_assign_id(AssetManager *m);
