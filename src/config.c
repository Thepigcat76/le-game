#include "../include/config.h"
#include "../include/shared.h"
#include "../include/cJSON.h"
#include <stdio.h>
#include <stdlib.h>

Config CONFIG;

void config_on_reload() {
    char *file = read_file_to_string("config/game.json");
    cJSON *json = cJSON_Parse(file);
    if (json == NULL) {
      printf("Error parsing JSON\n");
      exit(1);
    }

    cJSON *ambient_light = cJSON_GetObjectItemCaseSensitive(json, "ambient-light");
    cJSON *player_speed = cJSON_GetObjectItemCaseSensitive(json, "player-speed");

    if (cJSON_IsNumber(ambient_light)) {
        CONFIG.ambient_light = ambient_light->valuedouble;
    }

    if (cJSON_IsNumber(player_speed)) {
        CONFIG.player_speed = player_speed->valuedouble;
    }

    cJSON_Delete(json);
    free(file);
}