#include "../../include/array.h"
#include "../../include/game.h"
#include "../../vendor/cJSON.h"
#include <dirent.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static SaveConfig game_load_save_config(const char *path) {
  const char *file_path = TextFormat("%s/game.json", path);
  TraceLog(LOG_DEBUG, "Save config path: %s", file_path);
  char *file_content = read_file_to_string(file_path);
  cJSON *json = cJSON_Parse(file_content);
  cJSON *save_name_json = cJSON_GetObjectItemCaseSensitive(json, "name");
  char *save_name;
  if (cJSON_IsString(save_name_json)) {
    save_name = malloc(strlen(save_name_json->valuestring) + 1);
    strcpy(save_name, save_name_json->valuestring);
  } else {
    TraceLog(LOG_WARNING, "Failed to get name for save");
  }
  cJSON *world_json = cJSON_GetObjectItemCaseSensitive(json, "world");
  cJSON *seed_json = cJSON_GetObjectItemCaseSensitive(world_json, "seed");
  SaveConfig config = {
      .save_name = save_name,
      .seed = (float)seed_json->valuedouble,
  };
  free(file_content);
  cJSON_Delete(json);
  return config;
}

void game_load_saves(Game *game) {
  if (!dir_exists(SAVE_DIR)) {
    dir_create(SAVE_DIR);
  }

  size_t saves_len = array_len(game->client_game->local_saves);

  if (saves_len > 0) {
    array_clear(game->client_game->local_saves);
  }

  DIR_ITER(SAVE_DIR, entry, {
    const char *full_dir_name = TextFormat("%s%s", SAVE_DIR, entry->d_name);

    TraceLog(LOG_DEBUG, "Dir entry: %s", full_dir_name);
    if (dir_exists(full_dir_name) && string_starts_with(entry->d_name, "save")) {
      int id = atoi(TextSubtext(entry->d_name, 4, strlen(entry->d_name) - 4));
      SaveConfig config = game_load_save_config(full_dir_name);
      SaveDescriptor desc = {.id = id, .config = config};
      array_add(game->client_game->local_saves, desc);
    }
  });
}

static void game_load_cur_save(Game *game, SaveDescriptor desc) {
  game_load_save_data(game, desc);
  game->world = &game->cur_save.cur_space->world;
  game->player = &game->cur_save.player;

  // TODO: world_prepare_rendering(game->world);
}

void game_load_save(Game *game, SaveDescriptor desc) {
  size_t saves_len = array_len(game->client_game->local_saves);
  for (int i = 0; i < saves_len; i++) {
    if (game->client_game->local_saves[i].id == desc.id) {
      game_load_cur_save(&GAME, desc);
      break;
    }
  }
}

void game_unload_save(Game *game) {
  game_save_save_data(game, &game->cur_save);

  bump_reset(&ITEM_CONTAINER_BUMP);
  game->world = NULL;
  game->player = NULL;
}

static void game_create_save_config_file(Game *game, SaveConfig config) {
  cJSON *json = cJSON_CreateObject();
  cJSON_AddStringToObject(json, "name", config.save_name);
  {
    cJSON *world_json = cJSON_AddObjectToObject(json, "world");
    cJSON_AddNumberToObject(world_json, "seed", config.seed);
  }

  // TO FILE
  char *json_str = cJSON_Print(json);
  size_t index = array_len(game->client_game->local_saves);
  FILE *fp = fopen(TextFormat("save/save%d/game.json", index), "w");
  if (fp) {
    fputs(json_str, fp);
    fclose(fp);
    printf("Successfully created game.json\n");
  } else {
    perror("Failed to open file");
  }
  free(json_str);
  cJSON_Delete(json);
}

void game_create_save_world(Game *game) {
  player_set_pos_ex(&game->cur_save.player, TILE_SIZE * ((float)CHUNK_SIZE / 2), TILE_SIZE * ((float)CHUNK_SIZE / 2), false, false, false);
}

void game_create_save(Game *game, SaveDescriptor save_desc) {
  if (!DirectoryExists("save")) {
    dir_create("save");
  }

  char *save_name_copy = malloc(strlen(save_desc.config.save_name) + 1);
  strcpy(save_name_copy, save_desc.config.save_name);
  // float seed = string_to_world_seed(seed_lit);
  // size_t id = array_len(game->local_saves);
  float seed = save_desc.config.seed;
  size_t id = save_desc.id;

  SaveDescriptor desc = {.id = id,
                         .config = {
                             .save_name = save_name_copy,
                             .seed = seed,
                         }};

  dir_create(TextFormat("save/save%d", id));
  game_create_save_config_file(game, desc.config);

  dir_create(TextFormat("save/save%d/spaces", id));

  array_add(game->client_game->local_saves, desc);
  game->cur_save = (Save){.descriptor = desc,
                          .player = player_new(),
                          .spaces = array_new_capacity(SpaceDescriptor, 8, &HEAP_ALLOCATOR),
                          .loaded_spaces = array_new_capacity(Space, 8, &HEAP_ALLOCATOR)};
  array_add(game->cur_save.spaces, (SpaceDescriptor){.type = &SPACES[SPACE_BASE], .id = 0});
  Space default_space;
  space_create_default(save_desc.config.seed, &default_space);
  array_add(game->cur_save.loaded_spaces, default_space);
  game->cur_save.cur_space = &game->cur_save.loaded_spaces[0];
  game->world = &game->cur_save.cur_space->world;
  game->player = &game->cur_save.player;
}
