#include "../../include/game.h"
#include "../../vendor/cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

static void game_create_save_config(Game *game, int save_index, const char *save_name, float seed) {
  cJSON *json = cJSON_CreateObject();
  cJSON_AddStringToObject(json, "name", save_name);
  {
    cJSON *world_json = cJSON_AddObjectToObject(json, "world");
    cJSON_AddNumberToObject(world_json, "seed", seed);
  }
  // TO FILE
  char *json_str = cJSON_Print(json);
  FILE *fp = fopen(TextFormat("save/save%d/game.json", save_index), "w");
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

void game_create_save(Game *game, const char *save_name, const char *seed_lit) {
  if (!DirectoryExists("save")) {
    create_dir("save");
  }

  game->cur_save = game->detected_saves;
  create_dir(TextFormat("save/save%d", game->cur_save));
  float seed = string_to_world_seed(seed_lit);
  game_create_save_config(game, game->cur_save, save_name, seed);
  game_create_world(&GAME, seed);
}

void game_cur_save_init(Game *game) { game->world.seed = game->save_configs[game->cur_save].seed; }

void game_detect_saves(Game *game) {
  if (!DirectoryExists("save")) {
    create_dir("save");
  }

  if (game->detected_saves > 0) {
    free(game->save_configs);
  }

  game->detected_saves = 0;
  DIR *dir = opendir(SAVE_DIR);

  if (dir == NULL) {
    perror("Failed to open save directory to load saves");
    exit(1);
  }

  struct dirent *entry;

  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    const char *full_dir_name = TextFormat("%s%s", SAVE_DIR, entry->d_name);

    TraceLog(LOG_DEBUG, "Dir entry: %s", full_dir_name);
    if (is_dir(full_dir_name) && string_starts_with(entry->d_name, "save")) {
      game->detected_saves++;
    }
  }

  closedir(dir);

  // TODO: Use dyn arrays
  game->save_configs = malloc(game->detected_saves * sizeof(GameConfig));
  for (int i = 0; i < game->detected_saves; i++) {
    GameConfig config;
    char *file_content = read_file_to_string(TextFormat("save/save%d/game.json", i));
    cJSON *json = cJSON_Parse(file_content);
    free(file_content);
    cJSON *json_save_name = cJSON_GetObjectItemCaseSensitive(json, "name");
    cJSON *json_world = cJSON_GetObjectItemCaseSensitive(json, "world");
    cJSON *json_seed = cJSON_GetObjectItemCaseSensitive(json_world, "seed");
    if (cJSON_IsNumber(json_seed)) {
      config.seed = json_seed->valuedouble;
    }
    if (cJSON_IsString(json_save_name)) {
      config.save_name = malloc(strlen(json_save_name->valuestring) + 1);
      strcpy(config.save_name, json_save_name->valuestring);
    }
    cJSON_Delete(json);
    game->save_configs[i] = config;
  }
}