#include "../include/assets.h"
#include "../include/shared.h"
#include "../include/net/client.h"
#include "lilc/alloc.h"
#include "lilc/log.h"
#include "lilc/array.h"
#include "lilc/bump.h"
#include "dirent.h"
#include <raylib.h>
#include <stdio.h>

cw_Texture cw_tex_by_id(AssetManager *asset_manager, AssetId id) {
  return asset_manager->textures[id];
}

inline cw_Texture cw_tex_by_handle(AssetManager *asset_manager, TextureHandle handle) {
  return cw_tex_by_id(asset_manager, TEX_IDS[handle]);
}

Texture2D tex_by_id(AssetManager *asset_manager, AssetId id) {
  cw_Texture tex = cw_tex_by_id(asset_manager, id);
  if (tex.kind == TEXTURE_STATIC) {
    return tex.var.texture_static;
  }
  return tex.var.texture_animated.texture;
}

inline Texture2D tex_by_handle(AssetManager *asset_manager, TextureHandle handle) {
  return tex_by_id(asset_manager, TEX_IDS[handle]);
}

cw_Texture cw_tex_by_tex_path(AssetManager *asset_manager, const char *tex_path) {
  cw_Texture *tex;
  array_foreach(asset_manager->textures, tex) {
    if (strcmp(tex->path, TextFormat(ASSETS_DIR TEXTURES_DIR "/%s.png", tex_path)) == 0) {
      return *tex;
    }
  }
  return (cw_Texture){0};
}

i32 cw_tex_cur_frame(const cw_Texture *texture) {
  if (texture->kind != TEXTURE_ANIMATED)
    return 0;
  return CLIENT_GAME.tex_manager.textures[texture->var.texture_animated.animated_texture_id].cur_frame;
}

i32 cw_tex_frame_height(const cw_Texture *texture) {
  if (texture->kind == TEXTURE_STATIC)
    return texture->var.texture_static.height;
  return texture->var.texture_animated.texture.height / texture->var.texture_animated.frames;
}

static void asset_manager_init(AssetManager *asset_manager) {
  bump_init(&asset_manager->asset_bump, sizeof(cw_Texture) * 8192);
  bump_allocator_init(&asset_manager->asset_bump_allocator, &asset_manager->asset_bump);

  asset_manager->textures = array_new_capacity(cw_Texture, 1024, &HEAP_ALLOCATOR);
  asset_manager->shaders = array_new_capacity(cw_Shader, 1024, &HEAP_ALLOCATOR);
  asset_manager->sounds = array_new_capacity(cw_Sound, 1024, &HEAP_ALLOCATOR);
}

typedef void (*AssetFileVisitFunc)(AssetManager *asset_manager, FileEntry file_entry);

static void asset_dir_walk(AssetManager *asset_manager, const char *path, AssetFileVisitFunc visit_func) {
  struct dirent *entry;
  DIR *dp = opendir(path);
  if (dp == NULL) {
    perror("opendir");
    fprintf(stderr, "Path: %s\n", path);
    exit(1);
  }

  while ((entry = readdir(dp)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    char dir_buf[256];
    sprintf(dir_buf, "%s/%s", path, entry->d_name);
    if (entry->d_type == DT_DIR) {
      asset_dir_walk(asset_manager, dir_buf, visit_func);
      continue;
    } else {
      const char *dot = strrchr(entry->d_name, '.');
      const char *file_ext = NULL;
      if (dot != NULL) {
        file_ext = dot + 1;
      }
      char name[strlen(entry->d_name)];
      size_t length = dot - entry->d_name;
      strncpy(name, entry->d_name, dot - entry->d_name);
      name[length + 1] = '\0';
      FileEntry file_entry = {
          .dir = path,
          .name = entry->d_name,
          .file_ext = file_ext,
          .full_path = dir_buf,
      };
      visit_func(asset_manager, file_entry);
    }
  }

  closedir(dp);
}

static void texture_asset_visit(AssetManager *asset_manager, FileEntry file_entry) {
  if (!str_eq(file_entry.file_ext, "png")) return;

  cw_Texture texture = {0};
  cw_texture_load(&texture, asset_manager, file_entry);

  texture.id = array_len(asset_manager->textures);
  array_add(asset_manager->textures, texture);

  log_info("Loaded texture %s", texture.path);
}

void assets_load(AssetManager *asset_manager) {
  if (asset_manager->textures == NULL) {
    asset_manager_init(asset_manager);
  }

  asset_dir_walk(asset_manager, ASSETS_DIR TEXTURES_DIR, texture_asset_visit);

  texture_handles_assign_id(asset_manager);
}

void assets_unload(AssetManager *asset_manager) {
  cw_Texture *tex;
  array_foreach(asset_manager->textures, tex) {
    cw_texture_unload(tex);
  }

  bump_reset(&asset_manager->asset_bump);
}
