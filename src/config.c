#include "../include/config.h"
#include "../include/game.h"
#include "../include/shared.h"
#include "../vendor/cJSON.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Config CONFIG;

static void game_config_on_reload() {
  CONFIG_READ("game", json, {
    cJSON *ambient_light = cJSON_GetObjectItemCaseSensitive(json, "ambient-light");
    cJSON *player_speed = cJSON_GetObjectItemCaseSensitive(json, "player-speed");
    cJSON *default_font_size = cJSON_GetObjectItemCaseSensitive(json, "default-font-size");
    cJSON *interaction_range = cJSON_GetObjectItemCaseSensitive(json, "interaction-range");
    cJSON *item_pickup_delay = cJSON_GetObjectItemCaseSensitive(json, "item-pickup-delay");

    if (cJSON_IsNumber(ambient_light)) {
      CONFIG.ambient_light = ambient_light->valuedouble;
    }

    if (cJSON_IsNumber(player_speed)) {
      CONFIG.player_speed = player_speed->valuedouble;
    }

    if (cJSON_IsNumber(default_font_size)) {
      CONFIG.default_font_size = default_font_size->valueint;
    }

    if (cJSON_IsNumber(interaction_range)) {
      CONFIG.interaction_range = interaction_range->valueint;
    }

    if (cJSON_IsNumber(item_pickup_delay)) {
      CONFIG.item_pickup_delay = item_pickup_delay->valuedouble;
    }
  });
}

void config_on_reload(Game *game) {
  game_config_on_reload();
}
