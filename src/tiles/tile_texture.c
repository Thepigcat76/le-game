#include "../../include/net/client.h"
#include "../../include/shared.h"
#include "../../include/tile.h"
#include "../../vendor/cJSON.h"
#include "../../vendor/stb_perlin.h"
#include "lilc/log.h"
#include <dirent.h>
#include <lilc/alloc.h>
#include <lilc/file.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

static void init_connected_info(void);

static void init_tile_variants(void);

static void debug_variant_info(void);

static void on_tile_variants_reload(void);

void tiles_reload(ClientGame *client) { init_connected_info(); }

// -- CONNECTED TEXTURES --

typedef struct {
  i32 predicates[8];
  i32 predicates_amount;
  i32 ignored_tiles[8];
  i32 ignored_tiles_amount;
  Vec2i sprite_pos;
} Connection;

typedef struct {
  u32 res;
  Vec2i default_sprite_pos;
  Connection connections[256];
  size_t connections_amount;
} ConnectedInfo;

static ConnectedInfo CONNECTED_INFO = {0};

static void init_connected_info(void) {
  ConnectedInfo info = {0};
  dyn_string_t file = file_read_to_string("res/assets/connected.json", &HEAP_ALLOCATOR);
  cJSON *json = cJSON_Parse(file.string);
  if (json == NULL) {
    printf("Error parsing JSON\n");
    exit(1);
  }

  cJSON *res = cJSON_GetObjectItemCaseSensitive(json, "res");
  cJSON *default_sprite_pos = cJSON_GetObjectItemCaseSensitive(json, "default");
  cJSON *values = cJSON_GetObjectItemCaseSensitive(json, "values");

  if (cJSON_IsNumber(res)) {
    info.res = res->valueint;
  }

  if (cJSON_IsArray(default_sprite_pos)) {
    cJSON *x = cJSON_GetArrayItem(default_sprite_pos, 0);
    cJSON *y = cJSON_GetArrayItem(default_sprite_pos, 1);
    if (cJSON_IsNumber(x) && cJSON_IsNumber(y)) {
      info.default_sprite_pos = vec2i(x->valueint, y->valueint);
    } else {
      printf("Failed to get default sprite pos");
      exit(1);
    }
  }

  if (cJSON_IsArray(values)) {
    int size = cJSON_GetArraySize(values);
    size_t index = 0;
    for (int i = 0; i < size; i++) {
      cJSON *entry = cJSON_GetArrayItem(values, i);
      if (cJSON_IsString(entry)) {
        if (strncmp(entry->valuestring, "_comment", 8) == 0) {
          continue;
        } else {
          log_error("Invalid json element in connected.json: %s", entry->valuestring);
          exit(1);
        }
      }
      cJSON *tiles = cJSON_GetObjectItemCaseSensitive(entry, "tiles");
      cJSON *value = cJSON_GetObjectItemCaseSensitive(entry, "value");
      if (cJSON_HasObjectItem(entry, "ignored_tiles")) {
        cJSON *ignored = cJSON_GetObjectItemCaseSensitive(entry, "ignored_tiles");
        int j = 0;
        cJSON *elem;
        cJSON_ArrayForEach(elem, ignored) {
          if (cJSON_IsNumber(elem)) {
            info.connections[index].ignored_tiles[j] = elem->valueint;
            j++;
          }
        }
        info.connections[index].ignored_tiles_amount = j;
      }

      if (cJSON_IsArray(tiles)) {
        int size = cJSON_GetArraySize(tiles);
        for (int j = 0; j < size; j++) {
          cJSON *entry = cJSON_GetArrayItem(tiles, j);

          if (cJSON_IsNumber(entry)) {
            info.connections[index].predicates[j] = entry->valueint;
          } else {
          }
          info.connections[index].predicates_amount = size;
        }
      }

      Vec2i value_pos;

      if (cJSON_IsArray(value)) {
        cJSON *x = cJSON_GetArrayItem(value, 0);
        cJSON *y = cJSON_GetArrayItem(value, 1);
        if (cJSON_IsNumber(x) && cJSON_IsNumber(y)) {
          value_pos = vec2i(x->valueint, y->valueint);
        } else {
          printf("Failed to get sprite pos, values index: %d", i);
          exit(1);
        }
      }

      info.connections[index].sprite_pos = value_pos;
      index++;
    }
    info.connections_amount = size;
  }

  // DEBUG ignored tiles
  // for (int i = 0; i < info.connections_amount; i++) {
  //  Connection *connection = &info.connections[i];
  //  if (connection->ignored_tiles_amount > 0) {
  //    TraceLog(LOG_DEBUG, "!!Ignored tiles for %d %d!!", connection->sprite_pos.x, connection->sprite_pos.y);
  //    for (int j = 0; j < connection->ignored_tiles_amount; j++) {
  //      int ignored_tile = connection->ignored_tiles[j];
  //      TraceLog(LOG_DEBUG, "%d", ignored_tile);
  //    }
  //  }
  //}

  CONNECTED_INFO = info;

  cJSON_Delete(json);
  dyn_string_free(&file);
}

static bool tile_is_ignored(int *ignored_tiles, int ignored_tiles_amount, int ignored_index) {
  for (int i = 0; i < ignored_tiles_amount; i++) {
    if (ignored_tiles[i] == ignored_index) {
      return true;
    }
  }
  return false;
}

static void indices_arr_to_bool_arr(int *indices, int indices_amount, bool *arr, int arr_size) {
  for (int i = 0; i < indices_amount; i++) {
    int idx = indices[i];
    if (idx >= 0 && idx < arr_size) {
      arr[idx] = true;
    }
  }
}

static bool cmp_same_tiles(bool *expected_same_tiles, bool *physical_same_tiles, int *ignored_tiles, int ignored_tiles_amount) {
  for (int i = 0; i < 8; i++) {
    if (tile_is_ignored(ignored_tiles, ignored_tiles_amount, i))
      continue;

    if (expected_same_tiles[i] != physical_same_tiles[i]) {
      return false;
    }
  }
  return true;
}

static bool indices_true_and_other_false(bool *physical_same_tiles, int *indices, int indices_amount, int *ignored, int ignored_amount,
                                         int size) {
  bool expected_same_tiles[8] = {false};
  indices_arr_to_bool_arr(indices, indices_amount, expected_same_tiles, 8);
  bool same_tiles = cmp_same_tiles(expected_same_tiles, physical_same_tiles, ignored, ignored_amount);
  return same_tiles;
}

static Rectangle sprite_rect(int x, int y) { return (Rectangle){.x = x, .y = y, .width = 16, .height = 16}; }

static Rectangle select_tile(bool *same_tile) {
  for (size_t i = 0; i < CONNECTED_INFO.connections_amount; i++) {
    Connection connection = CONNECTED_INFO.connections[i];
    if (indices_true_and_other_false(same_tile, connection.predicates, connection.predicates_amount, connection.ignored_tiles,
                                     connection.ignored_tiles_amount, 8)) {
      return sprite_rect(connection.sprite_pos.x, connection.sprite_pos.y);
    }
  }

  log_error("Failed to select tile box");
  return sprite_rect(CONNECTED_INFO.default_sprite_pos.x, CONNECTED_INFO.default_sprite_pos.y);
}

void tile_calc_sprite_box(TileInstance *tile) {
  TileProperties tile_props = CLIENT_GAME.game.registries.tiles[tile->id];

  if (tile_props.uses_tileset) {
    TileId self_id = tile->id;
    TileId *texture_data = tile->texture_data.surrounding_tiles;
    bool same_tile[8];
    same_tile[0] = texture_data[0] == self_id;
    same_tile[1] = texture_data[1] == self_id;
    same_tile[2] = texture_data[2] == self_id;
    same_tile[3] = texture_data[3] == self_id;
    same_tile[4] = texture_data[4] == self_id;
    same_tile[5] = texture_data[5] == self_id;
    same_tile[6] = texture_data[6] == self_id;
    same_tile[7] = texture_data[7] == self_id;
    tile->cur_sprite_box = select_tile(same_tile);
  }
}

Vec2i tile_default_sprite_pos() { return CONNECTED_INFO.default_sprite_pos; }

int tile_default_sprite_resolution() { return CONNECTED_INFO.res; }

// -- TEXTURE VARIANTS --

void tile_tex_manager_load(TileTextureManager *tile_tex_manager, const RegistryManager *registries, AssetManager *asset_manager) {
  if (tile_tex_manager->tile_variant_textures == NULL) {
    tile_tex_manager->tile_variant_textures = array_new(VariantTexture, &HEAP_ALLOCATOR);
  }

  TileProperties *tile;
  array_foreach(registries->tiles, tile) {
    cw_Texture tex = asset_manager->textures[tile->texture];
    VariantTexture var_tex = {
        .id = tile->id,
    };
    TextureMetaInfo meta_info = tex.meta_info;
    if (tex.has_meta_info && meta_info.has_variants) {
      var_tex.kind = TEX_VAR_SINGLE;
      var_tex.variants = array_new(AssetId, &asset_manager->asset_bump_allocator);
      
      char **variant_path;
      array_foreach(meta_info.variant_paths, variant_path) {
        cw_Texture tex = tex_by_tex_path(asset_manager, *variant_path);
        array_add(var_tex.variants, tex.id);

        log_debug("TExture: %s <-> id: %zu", tex.path, tex.id);
      }
    } else {
      var_tex.kind = TEX_VAR_NONE;
    }
    array_add(tile_tex_manager->tile_variant_textures, var_tex);
  }
}

void tile_tex_manager_unload(TileTextureManager *tile_tex_manager, const RegistryManager *registries, const AssetManager *asset_manager) {
  array_clear(tile_tex_manager->tile_variant_textures);
}

cw_Texture tile_tex(TileId id, TilePos tile_pos, f32 world_seed, const TileTextureManager *tile_texs, const RegistryManager *registries,
                   const AssetManager *asset_manager) {
  // We are just indexing arrays here, both operations should be relatively cheap
  TileProperties tile_props = registries->tiles[id];

  cw_Texture tex = tex_by_id(asset_manager, tile_props.texture);

  f32 seed_offset = world_seed * 37.77f;

  if (tex.meta_info.has_variants) {
    VariantTexture var_tex = tile_texs->tile_variant_textures[id];

    f32 fx = tile_pos.x * 0.1 + seed_offset;
    f32 fy = tile_pos.y * 0.1 + seed_offset;

    size_t variants = array_len(tile_texs->tile_variant_textures[id].variants);

    f32 raw_noise = (stb_perlin_noise3(fx, fy, 0.0f, 0, 0, 0) + 1);
    raw_noise = fminf(1.0f, fmaxf(0.0f, raw_noise));
    f32 noise = raw_noise * (variants - 1);

    return tex_by_id(asset_manager, var_tex.variants[(i32) noise]);
  }

  return tex;
}
