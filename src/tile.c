#include "../include/tile.h"
#include "../include/cJSON.h"
#include "../include/shared.h"
#include <raylib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  int predicate[8];
  int predicates;
  Vec2i sprite_pos;
} Connection;

typedef struct {
  int res;
  Vec2i default_sprite_pos;
  Connection connections[100];
  int connections_amount;
} ConnectedInfo;

TileType TILES[TILES_AMOUNT];
static ConnectedInfo CONNECTED_INFO;

// TILE TYPE

#define INIT_TILE(src_file_name)                                               \
  extern void src_file_name##_tile_init();                                     \
  src_file_name##_tile_init();

void init_connected_info() {
  ConnectedInfo info;
  char *file = read_file_to_string("res/connected.json");
  cJSON *json = cJSON_Parse(file);
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
    for (int i = 0; i < size; i++) {
      cJSON *entry = cJSON_GetArrayItem(values, i);
      cJSON *tiles = cJSON_GetObjectItemCaseSensitive(entry, "tiles");
      cJSON *value = cJSON_GetObjectItemCaseSensitive(entry, "value");

      if (cJSON_IsArray(tiles)) {
        int size = cJSON_GetArraySize(tiles);
        for (int j = 0; j < size; j++) {
          cJSON *entry = cJSON_GetArrayItem(tiles, j);

          if (cJSON_IsNumber(entry)) {
            info.connections[i].predicate[j] = entry->valueint;
          } else {
          }
          info.connections[i].predicates = size;
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

      info.connections[i].sprite_pos = value_pos;
    }
    info.connections_amount = size;
  }

  CONNECTED_INFO = info;

  cJSON_Delete(json);
  free(file);
}

void tile_types_init() {
  init_connected_info();

  tile_type_init(TILE_EMPTY);
  tile_type_init(TILE_DIRT);
  tile_type_init(TILE_GRASS);
  tile_type_init(TILE_STONE);
}

void tile_type_init(TileId id) {
  switch (id) {
  case TILE_EMPTY: {
    INIT_TILE(empty)
    break;
  }
  case TILE_DIRT: {
    INIT_TILE(dirt)
    break;
  }
  case TILE_GRASS: {
    INIT_TILE(grass)
    break;
  }
  case TILE_STONE: {
    INIT_TILE(stone)
    break;
  }
  }
}

char *tile_type_to_string(TileType *type) {
  switch (type->id) {
  case TILE_EMPTY:
    return "empty";
  case TILE_GRASS:
    return "grass";
  case TILE_STONE:
    return "stone";
  case TILE_DIRT:
    return "dirt";
  }
}

// TILE INSTANCE

static Rectangle rect(int x, int y, int w, int h) {
  return (Rectangle){.x = x, .y = y, .width = w, .height = h};
}

TileInstance tile_new(const TileType *type, int x, int y) {
  Vec2i default_pos = CONNECTED_INFO.default_sprite_pos;
  int default_sprite_res = CONNECTED_INFO.res;
  TileInstance instance = {
      .type = *type,
      .box = {.width = TILE_SIZE,
              .height = TILE_SIZE,
              .x = (float)x,
              .y = (float)y},
      .custom_data = {},
      .cur_sprite_box =
          type->uses_tileset
              ? rect(default_pos.x, default_pos.y, default_sprite_res,
                     default_sprite_res)
              : rect(0, 0, type->texture.width, type->texture.height),
  };
  return instance;
}

#define IS_MAIN_TILE(i) (i == 1 || i == 3 || i == 4 || i == 6)

static bool indices_true_and_other_false(bool *arr, int indices[],
                                         int indices_amount, int size) {
  for (int i = 0; i < size; i++) {
    bool is_selected = false;
    for (int j = 0; j < indices_amount; j++) {
      if (i == indices[j]) {
        is_selected = true;
        break;
      }
    }
    if (is_selected) {
      if (!arr[i])
        return false; // selected index must be true
    } else {
      if (arr[i] && IS_MAIN_TILE(i))
        return false; // non-selected index must be false
    }
  }
  return true;
}

static Rectangle sprite_rect(int x, int y) {
  return (Rectangle){.x = x, .y = y, .width = 16, .height = 16};
}

// TODO: Cache sprite
static Rectangle select_tile(bool *same_tile) {
  for (int i = 0; i < CONNECTED_INFO.connections_amount; i++) {
    Connection connection = CONNECTED_INFO.connections[i];
    if (indices_true_and_other_false(same_tile, connection.predicate,
                                     connection.predicates, 8)) {
      return sprite_rect(connection.sprite_pos.x, connection.sprite_pos.y);
    }
  }

  TraceLog(LOG_ERROR, "Failed to select tile box");
  return sprite_rect(CONNECTED_INFO.default_sprite_pos.x,
                     CONNECTED_INFO.default_sprite_pos.y);
}

void tile_calc_sprite_box(TileInstance *tile) {
  if (tile->type.uses_tileset) {
    TileId self_id = tile->type.id;
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

void tile_render(const TileInstance *tile) {
  if (tile->type.has_texture) {
    DrawTextureRec(tile->type.texture, tile->cur_sprite_box,
                   (Vector2){tile->box.x, tile->box.y}, WHITE);
  }
}

void tile_right_click(TileInstance *tile) {}

void tile_tick(TileInstance *tile) {}

void tile_load(TileInstance *tile, const DataMap *data) {

}

void tile_save(const TileInstance *tile, DataMap *data) {}
