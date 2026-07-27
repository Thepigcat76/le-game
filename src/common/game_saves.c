#include "lilc/array.h"
#include "../../include/game.h"
#include "../../include/net/client.h"
#include "../../vendor/cJSON.h"
#include <dirent.h>
#include <lilc/alloc.h>
#include <lilc/file.h>
#include <lilc/dir.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static SaveConfig game_load_save_config(const char *path) {
  const char *file_path = TextFormat("%s/game.json", path);
  TraceLog(LOG_DEBUG, "Save config path: %s", file_path);
  dyn_string_t file_content = file_read_to_string(file_path, &HEAP_ALLOCATOR);
  cJSON *json = cJSON_Parse(file_content.string);
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
  dyn_string_free(&file_content);
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
      SaveDescriptor desc = {.id = id, .config = config, .is_server_save = false};
      array_add(game->client_game->local_saves, desc);
    }
  });
}

static void game_load_cur_save(Game *game, SaveDescriptor desc) {
  game_load_save_data(game, desc);
  game->client_game->world = &game->cur_save.loaded_spaces[0].world;
}

void game_load_save(Game *game, SaveDescriptor desc) {
  size_t saves_len = array_len(game->client_game->local_saves);
  for (int i = 0; i < saves_len; i++) {
    if (game->client_game->local_saves[i].id == desc.id) {
      game_load_cur_save(game, desc);
      break;
    }
  }
}

void game_unload_save(Game *game) {
  if (GAME_SIDE == SIDE_SERVER || !game->cur_save.descriptor.is_server_save) {
    game_save_save_data(game, &game->cur_save);
  }

  bump_reset(&ITEM_CONTAINER_BUMP);
  game->client_game->world = NULL;
}

static void game_create_save_config_file(Game *game, SaveConfig config) {
  cJSON *json = save_config_to_json(&config);
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
  for (size_t i = 0; i < 1; i++) {
    float x = TILE_SIZE * ((float)CHUNK_SIZE / 2);
    float y = TILE_SIZE * ((float)CHUNK_SIZE / 2);
    player_set_pos_ex(&game->client_game->cur_player, x, y, false, false, false);
  }
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
                         .config =
                             {
                                 .save_name = save_name_copy,
                                 .seed = seed,
                             },
                         .is_server_save = save_desc.is_server_save};

  dir_create(TextFormat("save/save%d", id));
  game_create_save_config_file(game, desc.config);

  dir_create(TextFormat("save/save%d/spaces", id));

  array_add(game->client_game->local_saves, desc);
  game->cur_save = save_new(desc);
  Player player = {0};
  player_init(&player);
  game->client_game->cur_player = player;
  //array_add(game->cur_save.players, player);
  array_add(game->cur_save.spaces, (SpaceDescriptor){.space_id = SPACE_BASE, .id = 0});
  Space default_space;
  printf("Save Seed: %f\n", save_desc.config.seed);
  space_init_default(&default_space, save_desc.config.seed);
  array_add(game->cur_save.loaded_spaces, default_space);
  // game->cur_save.cur_space = &game->cur_save.loaded_spaces[0];
  // FIXME: Highly sus
  game->client_game->world = &game->cur_save.loaded_spaces[0].world;
  game->client_game->space = &game->cur_save.loaded_spaces[0];
}
