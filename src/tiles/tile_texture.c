#include "../../include/net/client.h"
#include "../../include/shared.h"
#include "../../vendor/cJSON.h"
#include "../../vendor/stb_perlin.h"
#include "lilc/log.h"
#include <dirent.h>
#include <lilc/alloc.h>
#include <lilc/array.h>
#include <lilc/file.h>
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>

// -- CONNECTED TEXTURES --

static void connected_info_load(TileTextureManager *tile_tex_manager, const char *file_path) {
  dyn_string_t file = file_read_to_string("res/assets/connected.json", &HEAP_ALLOCATOR);
  cJSON *json = cJSON_Parse(file.string);
  if (json == NULL) {
    log_error("Error parsing connected texture info file at: %s", file_path);
    return;
  }

  cJSON *res = cJSON_GetObjectItemCaseSensitive(json, "res");
  if (cJSON_IsNumber(res)) {
    tile_tex_manager->default_resolution = res->valueint;
  }

  cJSON *default_sprite_pos = cJSON_GetObjectItemCaseSensitive(json, "default");
  if (cJSON_IsArray(default_sprite_pos)) {
    cJSON *x = cJSON_GetArrayItem(default_sprite_pos, 0);
    cJSON *y = cJSON_GetArrayItem(default_sprite_pos, 1);
    if (cJSON_IsNumber(x) && cJSON_IsNumber(y)) {
      tile_tex_manager->default_sprite_pos = vec2i(x->valueint, y->valueint);
    } else {
      log_error("Failed to get default sprite pos");
      exit(1);
    }
  }

  if (tile_tex_manager->connections == NULL) {
    tile_tex_manager->connections = array_new_capacity(Connection, 128, &HEAP_ALLOCATOR);
  }

  cJSON *values = cJSON_GetObjectItemCaseSensitive(json, "values");
  if (cJSON_IsArray(values)) {

    cJSON *entry;
    cJSON_ArrayForEach(entry, values) {
      Connection connection = {0};

      if (cJSON_IsString(entry)) {
        if (strncmp(entry->valuestring, "_comment", 8) != 0) {
          log_error("Invalid json element in connected.json: %s", entry->valuestring);
        }
        continue;
      }

      if (cJSON_HasObjectItem(entry, "ignored_tiles")) {
        cJSON *ignored = cJSON_GetObjectItemCaseSensitive(entry, "ignored_tiles");
        i32 j = 0;
        cJSON *elem;
        cJSON_ArrayForEach(elem, ignored) {
          if (cJSON_IsNumber(elem)) {
            connection.ignored_tiles[j] = elem->valueint;
            j++;
          } else {
            log_error("[connected.json] Ignored entries are expected to be integers");
            continue;
          }
        }
        connection.ignored_tiles_amount = j;
      }

      cJSON *tiles = cJSON_GetObjectItemCaseSensitive(entry, "tiles");
      if (cJSON_IsArray(tiles)) {
        i32 size = cJSON_GetArraySize(tiles);
        for (i32 j = 0; j < size; j++) {
          cJSON *entry = cJSON_GetArrayItem(tiles, j);

          if (cJSON_IsNumber(entry)) {
            connection.predicates[j] = entry->valueint;
          } else {
            log_error("[connected.json] Predicate entries are expected to be integers");
            continue;
          }
          connection.predicates_amount = size;
        }
      }

      Vec2i value_pos;

      cJSON *value = cJSON_GetObjectItemCaseSensitive(entry, "value");

      bool valid_sprite_pos = false;

      if (cJSON_IsArray(value)) {
        cJSON *x = cJSON_GetArrayItem(value, 0);
        cJSON *y = cJSON_GetArrayItem(value, 1);
        if (cJSON_IsNumber(x) && cJSON_IsNumber(y)) {
          value_pos = vec2i(x->valueint, y->valueint);
          valid_sprite_pos = true;
        }
      }

      if (!valid_sprite_pos) {
        log_error("[connected.json] Sprite pos needs to be array of integers");
        continue;
      }

      connection.sprite_pos = value_pos;

      array_add(tile_tex_manager->connections, connection);
    }
  }

  cJSON_Delete(json);
  dyn_string_free(&file);
}

static bool tile_is_ignored(const i32 *ignored_tiles, i32 ignored_tiles_amount, i32 ignored_index) {
  for (i32 i = 0; i < ignored_tiles_amount; i++) {
    if (ignored_tiles[i] == ignored_index) {
      return true;
    }
  }
  return false;
}

static void indices_arr_to_bool_arr(const i32 *indices, i32 indices_amount, bool *arr, i32 arr_size) {
  for (i32 i = 0; i < indices_amount; i++) {
    i32 idx = indices[i];
    if (idx >= 0 && idx < arr_size) {
      arr[idx] = true;
    }
  }
}

static bool cmp_same_tiles(bool *expected_same_tiles, bool *physical_same_tiles, const i32 *ignored_tiles, i32 ignored_tiles_amount) {
  for (i32 i = 0; i < 8; i++) {
    if (tile_is_ignored(ignored_tiles, ignored_tiles_amount, i))
      continue;

    if (expected_same_tiles[i] != physical_same_tiles[i]) {
      return false;
    }
  }
  return true;
}

// Checks if if the predicates for a function match
static bool predicates_match(bool *physical_same_tiles, const Connection *connection) {
  bool expected_same_tiles[8] = {false};
  indices_arr_to_bool_arr(connection->predicates, connection->predicates_amount, expected_same_tiles, 8);
  bool same_tiles = cmp_same_tiles(expected_same_tiles, physical_same_tiles, connection->ignored_tiles, connection->ignored_tiles_amount);
  return same_tiles;
}

static Rectangle sprite_rect(i32 x, i32 y) { return (Rectangle){.x = x, .y = y, .width = 16, .height = 16}; }

static Rectangle select_tile(const TileTextureManager *tex_manager, bool *same_tile) {
  Connection *connection;
  array_foreach(tex_manager->connections, connection) {
    if (predicates_match(same_tile, connection)) {
      return sprite_rect(connection->sprite_pos.x, connection->sprite_pos.y);
    }
  }

  log_error("Failed to select tile box");
  return sprite_rect(tex_manager->default_sprite_pos.x, tex_manager->default_sprite_pos.y);
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
    tile->cur_sprite_box = select_tile(&CLIENT_GAME.tile_tex_manager, same_tile);
  }
}

// -- TEXTURE VARIANTS --

void tile_tex_manager_load(TileTextureManager *tile_tex_manager, const RegistryManager *registries, AssetManager *asset_manager) {
  if (tile_tex_manager->tile_variant_textures == NULL) {
    tile_tex_manager->tile_variant_textures = array_new(VariantTexture, &HEAP_ALLOCATOR);
  }

  connected_info_load(tile_tex_manager, "res/assets/connected.json");

  TileProperties *tile;
  array_foreach(registries->tiles, tile) {
    cw_Texture tex = asset_manager->textures[tile->texture];
    VariantTexture var_tex = {0};

    var_tex_load(&var_tex, &tex, asset_manager);

    array_add(tile_tex_manager->tile_variant_textures, var_tex);
  }
}

void tile_tex_manager_unload(TileTextureManager *tile_tex_manager, const RegistryManager *registries, const AssetManager *asset_manager) {
  array_clear(tile_tex_manager->tile_variant_textures);
  array_clear(tile_tex_manager->connections);
}

cw_Texture tile_tex(TileId id, TilePos tile_pos, f32 world_seed, const TileTextureManager *tile_texs, const RegistryManager *registries,
                    const AssetManager *asset_manager) {
  // We are just indexing arrays here, both operations should be relatively cheap
  TileProperties tile_props = registries->tiles[id];

  cw_Texture tex = tex_by_id(asset_manager, tile_props.texture);

  f32 seed_offset = world_seed * 37.77f;

  if (tex.meta_info.has_variants) {
    VariantTexture var_tex = tile_texs->tile_variant_textures[id];
    
    AssetId id = var_tex_for_pos(&var_tex, tile_pos.x, tile_pos.y, seed_offset);

    f32 fx = tile_pos.x * 0.1 + seed_offset;
    f32 fy = tile_pos.y * 0.1 + seed_offset;

    size_t variants = array_len(tile_texs->tile_variant_textures[id].variants);

    f32 raw_noise = stb_perlin_noise3(fx, fy, 0.0f, 0, 0, 0) + 1.0f;
    raw_noise = fminf(1.0f, fmaxf(0.0f, raw_noise));
    f32 noise = raw_noise * (variants - 1);

    return tex_by_id(asset_manager, var_tex.variants[(i32)noise]);
  }

  return tex;
}
