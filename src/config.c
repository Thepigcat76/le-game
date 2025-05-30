#include "../include/config.h"
#include "../include/shared.h"
#include "../vendor/cJSON.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Config CONFIG;
Keybinds KEYBINDS;

#define CONFIG_READ(file_name, json_object_name, block)                                                                \
  char *file = read_file_to_string("config/" file_name ".json");                                                       \
  cJSON *json_object_name = cJSON_Parse(file);                                                                         \
  if (json_object_name == NULL) {                                                                                      \
    printf("Error parsing JSON\n");                                                                                    \
    exit(1);                                                                                                           \
  }                                                                                                                    \
  {block};                                                                                                             \
  cJSON_Delete(json_object_name);                                                                                      \
  free(file);

static int key_from_str(char *key) {
  int keys_amount = 9;
  char *key_strings[] = {"W", "A", "S", "D", "R", "E", "B", "H", "Esc", "Up", "Down", "Left", "Right", "F3"};
  int keys[] = {KEY_W, KEY_A,      KEY_S,  KEY_D,    KEY_R,    KEY_E,     KEY_B,
                KEY_H, KEY_ESCAPE, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_F3};
  for (int i = 0; i < keys_amount; i++) {
    if (strcmp(key, key_strings[i]) == 0) {
      return keys[i];
    }
  }
  return 0;
}

static void keybinds_on_reload() {
  CONFIG_READ("keybinds", json, {
    cJSON *move_foreward_key = cJSON_GetObjectItemCaseSensitive(json, "move-foreward");
    cJSON *move_backward_key = cJSON_GetObjectItemCaseSensitive(json, "move-backward");
    cJSON *move_left_key = cJSON_GetObjectItemCaseSensitive(json, "move-left");
    cJSON *move_right_key = cJSON_GetObjectItemCaseSensitive(json, "move-right");
    cJSON *open_close_save_menu_key = cJSON_GetObjectItemCaseSensitive(json, "open-close-save-menu");
    cJSON *reload_key = cJSON_GetObjectItemCaseSensitive(json, "reload");
    cJSON *open_backpack_menu = cJSON_GetObjectItemCaseSensitive(json, "open-backpack-menu");
    cJSON *toggle_hitbox = cJSON_GetObjectItemCaseSensitive(json, "toggle-hitbox");
    cJSON *zoom_in = cJSON_GetObjectItemCaseSensitive(json, "zoom-in");
    cJSON *zoom_out = cJSON_GetObjectItemCaseSensitive(json, "zoom-out");
    cJSON *open_debug_menu = cJSON_GetObjectItemCaseSensitive(json, "open-debug-menu");

    if (cJSON_IsString(move_foreward_key))
      KEYBINDS.move_foreward_key = key_from_str(move_foreward_key->valuestring);

    if (cJSON_IsString(move_backward_key))
      KEYBINDS.move_backward_key = key_from_str(move_backward_key->valuestring);

    if (cJSON_IsString(move_left_key))
      KEYBINDS.move_left_key = key_from_str(move_left_key->valuestring);

    if (cJSON_IsString(move_right_key))
      KEYBINDS.move_right_key = key_from_str(move_right_key->valuestring);

    if (cJSON_IsString(open_close_save_menu_key))
      KEYBINDS.open_close_save_menu_key = key_from_str(open_close_save_menu_key->valuestring);

    if (cJSON_IsString(reload_key))
      KEYBINDS.reload_key = key_from_str(reload_key->valuestring);

    if (cJSON_IsString(open_backpack_menu))
      KEYBINDS.open_backpack_menu_key = key_from_str(open_backpack_menu->valuestring);

    if (cJSON_IsString(zoom_in))
      KEYBINDS.zoom_in = key_from_str(zoom_in->valuestring);

    if (cJSON_IsString(zoom_out))
      KEYBINDS.zoom_out = key_from_str(zoom_out->valuestring);
  });
}

void config_on_reload() {
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

  keybinds_on_reload();
}
