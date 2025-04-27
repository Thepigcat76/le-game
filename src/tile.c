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

static void init_connected_info() {
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

  // cJSON_Delete(json);
  free(file);
}

void tile_types_init() {
  tile_type_init(TILE_DIRT);
  tile_type_init(TILE_GRASS);
  tile_type_init(TILE_STONE);

  init_connected_info();
}

void tile_type_init(TileId id) {
  switch (id) {
  case TILE_EMPTY: {
    INIT_TILE(empty)
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
  case TILE_DIRT: {
    INIT_TILE(dirt)
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

TileInstance tile_new(TileType *type, int x, int y) {
  TileInstance instance = {
      .type = *type,
      .box = {.width = TILE_SIZE,
              .height = TILE_SIZE,
              .x = (float)x,
              .y = (float)y},
      .custom_data = {},
  };
  return instance;
}

#define IS_MAIN_TILE(i) (i == 1 || i == 3 || i == 4 || i == 6)
#define MAIN_TILES (int[]){1, 3, 4, 6}

static bool all_true(bool *arr, int size) {
  for (int i = 0; i < size; i++) {
    if (!arr[i] && IS_MAIN_TILE(i)) {
      return false;
    }
  }
  return true;
}

static bool all_false(bool *arr, int size) {
  for (int i = 0; i < size; i++) {
    if (arr[i] && IS_MAIN_TILE(i)) {
      return false;
    }
  }
  return true;
}

static bool index_true_and_other_false(bool *arr, int index, int size) {
  for (int i = 0; i < size; i++) {
    if (i == index) {
      if (!arr[i]) {
        return false;
      }
    } else if (arr[i] && IS_MAIN_TILE(i)) {
      return false;
    }
  }
  return true;
}

static bool indices_2_true_and_other_false(bool *arr, int index0, int index1,
                                           int size) {
  for (int i = 0; i < size; i++) {
    if (i == index0 || i == index1) {
      if (!arr[i]) {
        return false;
      }
    } else if (arr[i] && IS_MAIN_TILE(i)) {
      return false;
    }
  }
  return true;
}

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

static Rectangle rect(int x, int y, int w, int h) {
  return (Rectangle){.x = x, .y = y, .width = w, .height = h};
}

static Rectangle sprite_rect(int x, int y) {
  return (Rectangle){.x = x, .y = y, .width = 16, .height = 16};
}

// TODO: Cache sprite
static Rectangle select_tile(bool *same_tile) {
  // COMPLETLY SURROUNDED
  if (all_true(same_tile, 8)) {
    return sprite_rect(32, 16);
  } else if (all_false(same_tile, 8)) {
    return sprite_rect(0, 48);
  }
  // SINGLE TILE NEXT TO THIS TILE
  else if (index_true_and_other_false(same_tile, 1, 8)) {
    return sprite_rect(0, 32);
  } else if (index_true_and_other_false(same_tile, 3, 8)) {
    return sprite_rect(48, 48);
  } else if (index_true_and_other_false(same_tile, 4, 8)) {
    return sprite_rect(16, 48);
  } else if (index_true_and_other_false(same_tile, 6, 8)) {
    return sprite_rect(0, 0);
  }
  // TILE ON EITHER SIDE
  else if (indices_true_and_other_false(same_tile, (int[]){3, 4}, 2, 8)) {
    return sprite_rect(32, 48);
  } else if (indices_true_and_other_false(same_tile, (int[]){1, 6}, 2, 8)) {
    return sprite_rect(0, 16);
  }
  // CROSS
  else if (indices_true_and_other_false(same_tile, (int[]){1, 3, 4, 6}, 4, 8)) {
    return sprite_rect(32, 16);
  }
  // T-JUNCTION
  else if (indices_true_and_other_false(same_tile, (int[]){3, 4, 6}, 3, 8)) {
    return sprite_rect(32, 0);
  } else if (indices_true_and_other_false(same_tile, (int[]){1, 4, 6}, 3, 8)) {
    return sprite_rect(16, 16);
  } else if (indices_true_and_other_false(same_tile, (int[]){1, 3, 4}, 3, 8)) {
    return sprite_rect(32, 32);
  } else if (indices_true_and_other_false(same_tile, (int[]){1, 3, 6}, 3, 8)) {
    return sprite_rect(48, 16);
  }
  // CORNER
  else if (indices_true_and_other_false(same_tile, (int[]){3, 5, 6}, 3, 8)) {
    return sprite_rect(48, 0);
  } else if (indices_true_and_other_false(same_tile, (int[]){4, 6, 7}, 3, 8)) {
    return sprite_rect(16, 0);
  } else if (indices_true_and_other_false(same_tile, (int[]){1, 2, 4}, 3, 8)) {
    return sprite_rect(16, 32);
  } else if (indices_true_and_other_false(same_tile, (int[]){0, 1, 3}, 3, 8)) {
    return sprite_rect(48, 32);
  }
  // STUFFS
  else if (indices_true_and_other_false(same_tile, (int[]){1, 3}, 2, 8)) {
    return sprite_rect(48, 32);
  } else if (indices_true_and_other_false(same_tile, (int[]){3, 6}, 2, 8)) {
    return sprite_rect(48, 0);
  } else if (indices_true_and_other_false(same_tile, (int[]){6, 4}, 2, 8)) {
    return sprite_rect(16, 0);
  } else if (indices_true_and_other_false(same_tile, (int[]){4, 1}, 2, 8)) {
    return sprite_rect(16, 32);
  }

  TraceLog(LOG_ERROR, "Failed to select tile");
  return sprite_rect(0, 48);
}

void tile_render(TileInstance *tile) {
  if (!tile->type.uses_tileset) {
    DrawTextureEx(tile->type.texture, (Vector2){tile->box.x, tile->box.y}, 0, 1,
                  WHITE);
  } else {
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
    Rectangle selected_tile = select_tile(same_tile);
    DrawTextureRec(tile->type.texture, selected_tile,
                   (Vector2){tile->box.x, tile->box.y}, WHITE);
  }
}

void tile_right_click(TileInstance *tile) {}

void tile_tick(TileInstance *tile) {}

void tile_load(TileInstance *tile) {}

void tile_save(TileInstance *tile) {}
