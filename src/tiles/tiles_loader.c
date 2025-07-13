#include "../../include/res_loader.h"
#include "../../include/shared.h"
#include "../../include/array.h"
#include "../../include/tile.h"
#include "../../vendor/cJSON.h"
#include <raylib.h>

#define JSON_GET_OR_ERROR(json, key, assign, err_action, type_check_type)                                                                  \
  do {                                                                                                                                     \
    cJSON *elem = cJSON_GetObjectItem(json, key);                                                                                          \
    if (cJSON_Is##type_check_type(elem)) {                                                                                                 \
      assign;                                                                                                                              \
    } else {                                                                                                                               \
      err_action;                                                                                                                          \
    }                                                                                                                                      \
  } while (0)

#define JSON_GET_REQUIRED(json, key, assign, type_check_type)                                                                              \
  do {                                                                                                                                     \
    cJSON *elem = cJSON_GetObjectItem(json, key);                                                                                          \
    if (cJSON_Is##type_check_type(elem)) {                                                                                                 \
      assign;                                                                                                                              \
    } else {                                                                                                                               \
      PANIC_FMT("Failed to get required key \"%s\"", key);                                                                                 \
    }                                                                                                                                      \
  } while (0)

#define JSON_GET_OR_DEFAULT(json, key, assign, default_assign, type_check_type)                                                            \
  do {                                                                                                                                     \
    cJSON *elem = cJSON_GetObjectItem(json, key);                                                                                          \
    if (cJSON_Is##type_check_type(elem)) {                                                                                                 \
      assign;                                                                                                                              \
    } else {                                                                                                                               \
      default_assign;                                                                                                                      \
    }                                                                                                                                      \
  } while (0)

#define JSON_EITHER_OR_DEFAULT(json, key, a_type, a_assign, b_type, b_assign, default_assign)                                              \
  do {                                                                                                                                     \
    cJSON *elem = cJSON_GetObjectItem(json, key);                                                                                          \
    if (cJSON_Is##a_type(elem)) {                                                                                                          \
      a_assign;                                                                                                                            \
    } else if (cJSON_Is##b_type(elem)) {                                                                                                   \
      b_assign;                                                                                                                            \
    } else {                                                                                                                               \
      default_assign;                                                                                                                      \
    }                                                                                                                                      \
  } while (0)

static Dimensionsf tile_dim(cJSON *elem) {
  float dim_x = cJSON_GetArrayItem(elem, 0)->valuedouble;
  float dim_y = cJSON_GetArrayItem(elem, 1)->valuedouble;
  return dimf(dim_x, dim_y);
}

static Color color_from_arr(cJSON *elem) {
  size_t array_len = cJSON_GetArraySize(elem);
  if (array_len != 3)
    return RAYWHITE;

  int r = cJSON_GetArrayItem(elem, 0)->valueint;
  int g = cJSON_GetArrayItem(elem, 1)->valueint;
  int b = cJSON_GetArrayItem(elem, 2)->valueint;
  return color_rgb(r, g, b);
}

// TODO: Use bump instead of heap for str cpy

static void tile_load(const char *file_name, cJSON *json, TileType *type) {
  // TODO: Split the file name to obtain the actual id
  JSON_GET_OR_DEFAULT(json, "id", type->id_literal = str_cpy_heap(elem->valuestring), type->id_literal = str_cpy_heap(file_name), String);
  JSON_GET_OR_DEFAULT(json, "name", type->name = str_cpy_heap(elem->valuestring), type->name = type->id_literal, String);
  JSON_GET_REQUIRED(json, "layer", type->layer = tile_layer_from_str(elem->valuestring), String);
  JSON_GET_OR_DEFAULT(json, "texture", type->texture = adv_texture_load(elem->valuestring), type->texture = ERR_TEXTURE, String);
  JSON_GET_OR_DEFAULT(json, "has-texture", type->has_texture = cJSON_IsTrue(elem), type->has_texture = true, Bool);
  JSON_GET_OR_DEFAULT(json, "dimensions", type->tile_dimensions = tile_dim(elem), type->tile_dimensions = DEFAULT_TILE_DIMENSIONS, Array);
  JSON_GET_OR_DEFAULT(json, "tile-item", type->tile_item = item_from_str(elem->valuestring), type->tile_item = NULL, String);
  // NESTED TILES DONT WORK YET
  JSON_GET_OR_DEFAULT(
      json, "tile-props",
      {
        cJSON *props = elem;
        JSON_GET_OR_DEFAULT(props, "collisions", type->tile_props.disable_collisions = !cJSON_IsTrue(elem),
                            type->tile_props.disable_collisions = false, Bool);
        JSON_GET_OR_DEFAULT(props, "break-time", type->tile_props.break_time = elem->valueint, type->tile_props.break_time = 64, Number);
        JSON_EITHER_OR_DEFAULT(props, "tile-color", Array, type->tile_props.tile_color = color_from_arr(elem), String,
                               type->tile_props.tile_color = color_from_str(elem->valuestring), type->tile_props.tile_color = RAYWHITE);
      },
      type->tile_props = TILE_PROPS_DEFAULT, Object);
  JSON_GET_OR_DEFAULT(
      json, "texture-props",
      {
        cJSON *props = elem;
        JSON_GET_OR_DEFAULT(props, "uses-tileset", type->texture_props.uses_tileset = cJSON_IsTrue(elem),
                            type->texture_props.uses_tileset = false, Bool);
        JSON_GET_OR_DEFAULT(props, "uses-variants", type->texture_props.has_variants = cJSON_IsTrue(elem),
                            type->texture_props.has_variants = false, Bool);
      },
      type->texture_props = TILE_TEXTURE_PROPS_DEFAULT, Object);
}

void tiles_load(void) {
  DIR_ITER("res/data/tiles", entry, {
    const char *file_name = TextFormat("res/data/tiles/%s", entry->d_name);
    char *file = read_file_to_string(file_name);
    cJSON *tile_json = cJSON_Parse(file);
    if (tile_json == NULL) {
      PANIC_FMT("Failed to parse json file: %s", file_name);
    }
    printf("File: %s\n", file_name);
    TileType type;
    tile_load(entry->d_name, tile_json, &type);
    cJSON_Delete(tile_json);
    free(file);
    type.id = 0;
    array_add(TILES, type);
  });
}
