#include "../include/assets.h"
#include "../include/net/client.h"
#include "../include/shared.h"
#include "dirent.h"
#include "lilc/alloc.h"
#include "lilc/array.h"
#include "lilc/bump.h"
#include "lilc/file.h"
#include "lilc/log.h"
#include <lilc/dynstr.h>
#include <lilc/str.h>
#include <raylib.h>
#include <stdio.h>

/* TEXTURES */

inline cw_Texture tex_by_id(const AssetManager *asset_manager, AssetId id) { return asset_manager->textures[id]; }

inline cw_Texture tex_by_handle(const AssetManager *asset_manager, TextureHandle handle) {
  return tex_by_id(asset_manager, TEX_IDS[handle]);
}

cw_Texture tex_by_tex_path(const AssetManager *asset_manager, const char *tex_path) {
  cw_Texture *tex;
  array_foreach(asset_manager->textures, tex) {
    if (strcmp(tex->path, TextFormat(ASSETS_DIR TEXTURES_DIR "/%s.png", tex_path)) == 0) {
      return *tex;
    }
  }
  return (cw_Texture){0};
}

i32 cw_tex_cur_frame(const cw_Texture *texture) {
  if (texture->has_meta_info && texture->meta_info.has_animation)
    return CLIENT_GAME.tex_manager.animated_textures[texture->id].cur_frame;
  return 0;
}

i32 cw_tex_frame_height(const cw_Texture *texture) {
  if (texture->has_meta_info && texture->meta_info.has_animation)
    return texture->meta_info.animation.frame_height;
  return texture->height;
}

/* SHADERS */

inline cw_Shader cw_shader_by_id(AssetManager *asset_manager, AssetId id) { return asset_manager->shaders[id]; }

inline cw_Shader cw_shader_by_handle(AssetManager *asset_manager, ShaderHandle handle) {
  return cw_shader_by_id(asset_manager, SHADER_IDS[handle]);
}

cw_Shader cw_shader_by_shader_path(AssetManager *asset_manager, const char *shader_path) {
  cw_Shader *shader;
  array_foreach(asset_manager->shaders, shader) {
    if (strcmp(shader->base_path, TextFormat(ASSETS_DIR SHADERS_DIR "/%s", shader_path)) == 0) {
      return *shader;
    }
  }
  return (cw_Shader){0};
}

inline Shader shader_by_id(AssetManager *asset_manager, AssetId id) { return cw_shader_by_id(asset_manager, id).shader; }

inline Shader shader_by_handle(AssetManager *asset_manager, ShaderHandle id) { return shader_by_id(asset_manager, SHADER_IDS[id]); }

/* SOUNDS */

inline cw_Sound cw_sound_by_id(AssetManager *asset_manager, AssetId id) { return asset_manager->sounds[id]; }

inline cw_Sound cw_sound_by_handle(AssetManager *asset_manager, SoundHandle handle) {
  return cw_sound_by_id(asset_manager, SOUND_IDS[handle]);
}

cw_Sound cw_sound_by_sound_path(AssetManager *asset_manager, const char *sound_path) {
  cw_Sound *sound;
  array_foreach(asset_manager->sounds, sound) {
    if (strcmp(sound->path, TextFormat(ASSETS_DIR SOUNDS_DIR "/%s.wav", sound_path)) == 0) {
      return *sound;
    }
  }
  return (cw_Sound){0};
}

inline Sound sound_by_id(AssetManager *asset_manager, AssetId id) { return cw_sound_by_id(asset_manager, id).sound; }

inline Sound sound_by_handle(AssetManager *asset_manager, SoundHandle handle) { return sound_by_id(asset_manager, SOUND_IDS[handle]); }

static void asset_manager_init(AssetManager *asset_manager) {
  bump_init(&asset_manager->asset_bump, sizeof(cw_Texture) * 8192);
  bump_allocator_init(&asset_manager->asset_bump_allocator, &asset_manager->asset_bump);

  asset_manager->textures = array_new_capacity(cw_Texture, 1024, &HEAP_ALLOCATOR);
  asset_manager->shaders = array_new_capacity(cw_Shader, 1024, &HEAP_ALLOCATOR);
  asset_manager->sounds = array_new_capacity(cw_Sound, 1024, &HEAP_ALLOCATOR);
}

typedef void (*AssetFileVisitFunc)(AssetManager *asset_manager, FileEntry file_entry, FileEntry meta_file_entry);

typedef void (*ShaderAssetFileVisitFunc)(AssetManager *asset_manager, FileEntry vs_file_entry, FileEntry fs_file_entry,
                                         FileEntry meta_file_entry);

static bool shader_loaded(const AssetManager *asset_manager, const char *base_path) {
  cw_Shader *shader;
  array_foreach(asset_manager->shaders, shader) {
    if (str_eq(shader->base_path, base_path)) {
      return true;
    }
  }
  return false;
}

static void shaders_asset_dir_walk(AssetManager *asset_manager, const char *path, ShaderAssetFileVisitFunc visit_func) {
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
      shaders_asset_dir_walk(asset_manager, dir_buf, visit_func);
      continue;
    } else {
      const char *file_ext = file_extension(entry->d_name);
      if (file_ext == NULL) {
        continue;
      }

      if (str_eq(file_ext, "json")) {
        continue;
      }

      dyn_string_t name = file_name(entry->d_name, &HEAP_ALLOCATOR);

      if (shader_loaded(asset_manager, name.string)) {
        continue;
      }

      FileEntry vs_file_entry;
      FileEntry fs_file_entry;
      FileEntry meta_file_entry;

      FileEntry cur_entry;

      cur_entry.dir = path;
      cur_entry.name = name.string;
      cur_entry.file_ext = file_ext;
      cur_entry.full_path = dir_buf;

      char other_full_path_buf[256];
      if (str_eq(file_ext, "vs")) {
        sprintf(other_full_path_buf, "%s/%s.fs", path, name.string);

        vs_file_entry = cur_entry;

        fs_file_entry.dir = path;
        fs_file_entry.name = name.string;
        fs_file_entry.file_ext = "fs";
        fs_file_entry.full_path = other_full_path_buf;

        if (!file_exists(other_full_path_buf)) {
          memset(&fs_file_entry, 0, sizeof(FileEntry));
        }
      } else if (str_eq(file_ext, "fs")) {
        sprintf(other_full_path_buf, "%s/%s.vs", path, name.string);

        fs_file_entry = cur_entry;

        vs_file_entry.dir = path;
        vs_file_entry.name = name.string;
        vs_file_entry.file_ext = "vs";
        vs_file_entry.full_path = other_full_path_buf;

        if (!file_exists(other_full_path_buf)) {
          memset(&vs_file_entry, 0, sizeof(FileEntry));
        }
      } else {
        continue;
      }

      char meta_file_name_buf[128];
      sprintf(meta_file_name_buf, "%s_meta.json", name.string);

      char meta_file_full_path_buf[128];
      sprintf(meta_file_full_path_buf, "%s/%s_meta.json", path, name.string);

      meta_file_entry.dir = path;
      meta_file_entry.name = meta_file_name_buf;
      meta_file_entry.file_ext = "json";
      meta_file_entry.full_path = meta_file_full_path_buf;

      if (!file_exists(meta_file_full_path_buf)) {
        memset(&meta_file_entry, 0, sizeof(FileEntry));
      }

      visit_func(asset_manager, vs_file_entry, fs_file_entry, meta_file_entry);

      dyn_string_free(&name);
    }
  }

  closedir(dp);
}

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
      name[length] = '\0';
      FileEntry file_entry = {
          .dir = path,
          .name = name,
          .file_ext = file_ext,
          .full_path = dir_buf,
      };
      FileEntry meta_file_entry = {0};
      dyn_string_t meta_file_path = {0};
      dyn_string_init(&meta_file_path, &HEAP_ALLOCATOR);
      dyn_string_printf(&meta_file_path, "%s/%s_meta.json", path, name);

      log_debug("META FILE PATH: %s", meta_file_path.string);

      dyn_string_t meta_name = {0};

      if (file_exists(meta_file_path.string)) {
        meta_name = file_name(meta_file_path.string, &HEAP_ALLOCATOR);

        meta_file_entry.dir = path;
        meta_file_entry.name = meta_name.string;
        meta_file_entry.file_ext = "json";
        meta_file_entry.full_path = meta_file_path.string;
      }
      visit_func(asset_manager, file_entry, meta_file_entry);

      dyn_string_free(&meta_file_path);
      dyn_string_free(&meta_name);
    }
  }

  closedir(dp);
}

static void tex_meta_info_load(cJSON *json, TextureMetaInfo *meta_info, cw_Texture tex, AssetManager *assets) {
  log_debug("Loading meta info");

  if (cJSON_HasObjectItem(json, "animation")) {
    meta_info->has_animation = true;

    cJSON *animation = cJSON_GetObjectItemCaseSensitive(json, "animation");
    if (cJSON_HasObjectItem(animation, "frame-time")) {
      cJSON *frame_time = cJSON_GetObjectItemCaseSensitive(animation, "frame-time");
      meta_info->animation.frame_time = frame_time->valueint;
      meta_info->animation.frames = tex.height / tex.width;
      if (cJSON_HasObjectItem(animation, "frame-height")) {
        cJSON *frame_height = cJSON_GetObjectItemCaseSensitive(animation, "frame-height");
        meta_info->animation.frame_height = frame_height->valueint;
      } else {
        meta_info->animation.frame_height = tex.width;
      }
    }
  }

  if (cJSON_HasObjectItem(json, "variants")) {
    cJSON *variants = cJSON_GetObjectItemCaseSensitive(json, "variants");
    log_debug("VARIANTS META INFO: %d", cJSON_GetArraySize(variants));

    if (cJSON_GetArraySize(variants) > 0) {
      meta_info->has_variants = true;

      meta_info->variant_paths = array_new(char *, &HEAP_ALLOCATOR);

      cJSON *variant;
      cJSON_ArrayForEach(variant, variants) {
        array_add(meta_info->variant_paths, str_cpy(variant->valuestring, &assets->asset_bump_allocator));
        log_debug("Add variant: %s", variant->valuestring);
      }
    }
  }
}

static void texture_asset_visit(AssetManager *asset_manager, FileEntry file_entry, FileEntry meta_file_entry) {
  if (!str_eq(file_entry.file_ext, "png"))
    return;

  cw_Texture texture = {0};
  if (cw_texture_load(&texture, asset_manager, file_entry)) {
    texture.id = array_len(asset_manager->textures);

    log_info("Loaded texture %s", texture.path);

    if (meta_file_entry.name != NULL) {
      texture.has_meta_info = true;

      dyn_string_t meta_file_content = file_read_to_string(meta_file_entry.full_path, &HEAP_ALLOCATOR);

      cJSON *json = cJSON_Parse(meta_file_content.string);

      TextureMetaInfo meta_info = {0};
      tex_meta_info_load(json, &meta_info, texture, asset_manager);
      texture.meta_info = meta_info;

      cJSON_Delete(json);

      dyn_string_free(&meta_file_content);
    }

    array_add(asset_manager->textures, texture);
  }
}

static void shader_asset_visit(AssetManager *asset_manager, FileEntry vs_file_entry, FileEntry fs_file_entry, FileEntry meta_file_entry) {
  cw_Shader shader = {0};
  if (cw_shader_load(&shader, asset_manager, vs_file_entry, fs_file_entry, meta_file_entry)) {
    shader.id = array_len(asset_manager->shaders);
    array_add(asset_manager->shaders, shader);

    log_info("Loaded shader %s", shader.base_path);
  }
}

static void sound_asset_visit(AssetManager *asset_manager, FileEntry file_entry, FileEntry meta_file_entry) {
  if (!str_eq(file_entry.file_ext, "wav"))
    return;

  cw_Sound sound = {0};
  if (cw_sound_load(&sound, asset_manager, file_entry)) {
    sound.id = array_len(asset_manager->sounds);
    array_add(asset_manager->sounds, sound);

    log_info("Loaded sound %s", sound.path);
  }
}

void assets_load(AssetManager *asset_manager) {
  if (asset_manager->textures == NULL) {
    asset_manager_init(asset_manager);
  }

  asset_dir_walk(asset_manager, ASSETS_DIR TEXTURES_DIR, texture_asset_visit);
  shaders_asset_dir_walk(asset_manager, ASSETS_DIR SHADERS_DIR, shader_asset_visit);
  asset_dir_walk(asset_manager, ASSETS_DIR SOUNDS_DIR, sound_asset_visit);

  texture_handles_assign_id(asset_manager);
  shader_handles_assign_id(asset_manager);
  sound_handles_assign_id(asset_manager);
}

void assets_unload(AssetManager *asset_manager) {
  cw_Texture *tex;
  array_foreach(asset_manager->textures, tex) { cw_texture_unload(tex); }
  cw_Shader *shader;
  array_foreach(asset_manager->shaders, shader) { cw_shader_unload(shader); }

  array_clear(asset_manager->textures);
  array_clear(asset_manager->shaders);

  bump_reset(&asset_manager->asset_bump);
}
