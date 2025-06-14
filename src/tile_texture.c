#include "../include/game.h"
#include "../include/shared.h"
#include "../include/tile.h"
#include "../vendor/cJSON.h"
#include <dirent.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>

static void init_connected_info();

static void init_tile_variants();

static void debug_variant_info();

static void on_tile_variants_reload();

void tile_on_reload(Game *game) {
  init_connected_info();

  on_tile_variants_reload();

  for (int i = 0; i < TILES_AMOUNT; i++) {
    if (TILES[i].id != TILE_EMPTY) {
      tile_type_init(&TILES[i]);
    }
  }
}

// -- CONNECTED TEXTURES --

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

static ConnectedInfo CONNECTED_INFO;

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

  cJSON_Delete(json);
  free(file);
}

#define IS_MAIN_TILE(i) (i == 1 || i == 3 || i == 4 || i == 6)

static bool indices_true_and_other_false(bool *arr, int indices[], int indices_amount, int size) {
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

static Rectangle sprite_rect(int x, int y) { return (Rectangle){.x = x, .y = y, .width = 16, .height = 16}; }

static Rectangle select_tile(bool *same_tile) {
  for (int i = 0; i < CONNECTED_INFO.connections_amount; i++) {
    Connection connection = CONNECTED_INFO.connections[i];
    if (indices_true_and_other_false(same_tile, connection.predicate, connection.predicates, 8)) {
      return sprite_rect(connection.sprite_pos.x, connection.sprite_pos.y);
    }
  }

  TraceLog(LOG_ERROR, "Failed to select tile box");
  return sprite_rect(CONNECTED_INFO.default_sprite_pos.x, CONNECTED_INFO.default_sprite_pos.y);
}

void tile_calc_sprite_box(TileInstance *tile) {
  if (tile->type.texture_props.uses_tileset) {
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

Vec2i tile_default_sprite_pos() { return CONNECTED_INFO.default_sprite_pos; }

int tile_default_sprite_resolution() { return CONNECTED_INFO.res; }

// -- TEXTURE VARIANTS --

typedef struct {
  AdvTexture *variants;
  int variants_amount;
} SingleTileVariant;

typedef struct {
  Vec2i *cases;
  SingleTileVariant *case_variant;
} ConnectedTileVariant;

typedef struct {
  enum {
    TILE_VARIANT_SINGLE,
    TILE_VARIANT_CONNECTED,
  } type;
  union {
    SingleTileVariant single_tile_variant;
    ConnectedTileVariant connected_tile_variant;
  } var;
} TileVariant;

typedef struct {
  char *tile_texture_names[200];
  TileVariant variants[200];
  int tiles_amount;
} TileVariantInfo;

static TileVariantInfo VARIANT_INFO = {.tiles_amount = 0};

static void debug_variant_info() {
  TraceLog(LOG_DEBUG, "Printing debug info for tile variants");
  for (int i = 0; i < VARIANT_INFO.tiles_amount; i++) {
    printf("Tile texture name: %s : %d variants\n", VARIANT_INFO.tile_texture_names[i],
           VARIANT_INFO.variants[i].var.single_tile_variant.variants_amount);
  }
}

static void init_variant_info(cJSON *meta_json, char *texture_file_name) {
  TileVariant variant;

  if (cJSON_HasObjectItem(meta_json, "variants")) {
    cJSON *variants = cJSON_GetObjectItemCaseSensitive(meta_json, "variants");
    if (cJSON_IsArray(variants)) {
      TraceLog(LOG_DEBUG, "Loaded variants");
      int len = cJSON_GetArraySize(variants);
      variant.var.single_tile_variant.variants = malloc(len * sizeof(AdvTexture));
      variant.type = TILE_VARIANT_SINGLE;
      for (int i = 0; i < len; i++) {
        cJSON *element = cJSON_GetArrayItem(variants, i);
        if (cJSON_IsString(element)) {
          int path_max_len = sizeof(ASSETS_DIR) + strlen(element->valuestring) + 1;
          char path[path_max_len];
          snprintf(path, path_max_len, "%s%s", ASSETS_DIR, element->valuestring);
          variant.var.single_tile_variant.variants[i] = adv_texture_load(path);
        }
      }
      variant.var.single_tile_variant.variants_amount = len;
    } else {
      variant.var.single_tile_variant.variants_amount = 0;
    }
  } else {
    return;
  }

  int variant_index = -1;
  for (int i = 0; i < VARIANT_INFO.tiles_amount; i++) {
    const char *texture_name = VARIANT_INFO.tile_texture_names[i];
    if (strcmp(texture_name, texture_file_name) == 0) {

      variant_index = i;
    }
  }

  if (variant_index == -1) {
    char *texture_file_name_heap = malloc(strlen(texture_file_name) + 1);
    strcpy(texture_file_name_heap, texture_file_name);
    VARIANT_INFO.tile_texture_names[VARIANT_INFO.tiles_amount] = texture_file_name_heap;
    VARIANT_INFO.variants[VARIANT_INFO.tiles_amount] = variant;
    VARIANT_INFO.tiles_amount++;
  } else {
    VARIANT_INFO.variants[variant_index] = variant;
  }
}

void tile_variants_free() {
  for (int i = 0; i < VARIANT_INFO.tiles_amount; i++) {
    free(VARIANT_INFO.tile_texture_names[i]);
    switch (VARIANT_INFO.variants[i].type) {
    case TILE_VARIANT_SINGLE:
      for (int j = 0; j < VARIANT_INFO.variants[i].var.single_tile_variant.variants_amount; j++) {
        adv_texture_unload(VARIANT_INFO.variants[i].var.single_tile_variant.variants[j]);
      }
      free(VARIANT_INFO.variants[i].var.single_tile_variant.variants);
      break;
    case TILE_VARIANT_CONNECTED:
      break;
    }
  }
  VARIANT_INFO.tiles_amount = 0;
}

static void init_tile_variants() {
  struct dirent *entry;
  DIR *dir = opendir(ASSETS_DIR);
  if (dir == NULL) {
    perror("opendir");
    exit(1);
  }

  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    const char *file_ext = GetFileExtension(entry->d_name);
    if (file_ext == NULL || strcmp(file_ext, ".png") != 0) {
      continue;
    }

    TraceLog(LOG_DEBUG, "File name: %s", entry->d_name);

    int count;
    const char **string_parts = TextSplit(entry->d_name, '.', &count);
    char meta_file_name[256];
    snprintf(meta_file_name, 256, "%s_meta.json", string_parts[0]);
    char meta_file_path[256];
    snprintf(meta_file_path, 256, "%s%s", ASSETS_DIR, meta_file_name);
    if (FileExists(meta_file_path)) {
      TraceLog(LOG_DEBUG, "Meta file name: %s", meta_file_path);
      char *meta_file_content = read_file_to_string(meta_file_path);
      cJSON *meta_json = cJSON_Parse(meta_file_content);
      {
        char texture_path[512];
        snprintf(texture_path, 512, "%s%s", ASSETS_DIR, entry->d_name);
        TraceLog(LOG_DEBUG, "Variant texture path: %s", texture_path);
        init_variant_info(meta_json, texture_path);
      }
      free(meta_file_content);
      cJSON_Delete(meta_json);
    }
  }

  closedir(dir);
}

static void on_tile_variants_reload() {
  // FINE SINCE AT THE BEGINNING TILES AMOUNT IS 0
  tile_variants_free();

  init_tile_variants();

  debug_variant_info();

  TraceLog(LOG_DEBUG, "Variants: %d", VARIANT_INFO.tiles_amount);
}

AdvTexture *tile_variants_for_tile(const TileType *type, int x, int y) {
  return VARIANT_INFO.variants[type->variant_index].var.single_tile_variant.variants;
}

int tile_variants_index_for_name(const char *texture_path, int x, int y) {
  int amount = VARIANT_INFO.tiles_amount;
  for (int i = 0; i < amount; i++) {
    if (strcmp(VARIANT_INFO.tile_texture_names[i], texture_path) == 0) {
      return i;
    }
  }
  return -1;
}

AdvTexture *tile_variants_by_index(int index, int x, int y) {
  return VARIANT_INFO.variants[index].var.single_tile_variant.variants;
}

int tile_variants_amount_for_tile(const TileType *type, int x, int y) {
  return VARIANT_INFO.variants[type->variant_index].var.single_tile_variant.variants_amount;
}

int tile_variants_amount_by_index(int index, int x, int y) {
  return VARIANT_INFO.variants[index].var.single_tile_variant.variants_amount;
}

// -- SHARED --

void tile_type_init(TileType *type) {
  int amount = VARIANT_INFO.tiles_amount;
  for (int i = 0; i < amount; i++) {
    if (strcmp(VARIANT_INFO.tile_texture_names[i], type->texture.path) == 0) {
      type->variant_index = i;
      return;
    }
  }
  type->variant_index = -1;
}
