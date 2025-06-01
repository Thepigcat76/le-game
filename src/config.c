#include "../include/config.h"
#include "../include/game.h"
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

#define KEYS_AMOUNT 105

// clang-format off
static const char *KEY_LITERALS[] = {
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
    "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T",
    "U", "V", "W", "X", "Y", "Z",
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    "Space", "Escape", "Enter", "Tab", "Backspace", "Insert", "Delete", 
    "Right", "Left", "Down", "Up",
    "PageUp", "PageDown", "Home", "End", 
    "CapsLock", "ScrollLock", "NumLock", 
    "PrintScreen", "Pause", 
    "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
    "LeftShift", "LeftControl", "LeftAlt", "LeftSuper", 
    "RightShift", "RightControl", "RightAlt", "RightSuper",
    "KbMenu",
    "KP0", "KP1", "KP2", "KP3", "KP4", "KP5", "KP6", "KP7", "KP8", "KP9",
    "KPDecimal", "KPDivide", "KPMultiply", "KPSubtract", "KPAdd", "KPEnter", "KPEqual",
    "Grave", "Slash", "Backslash", "Comma", "Period", "Semicolon", "Apostrophe",
    "LeftBracket", "RightBracket", "Minus", "Equal"
};

static const int KEY_IDS[] = {
    KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J,
    KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T,
    KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
    KEY_ZERO, KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE, KEY_SIX, KEY_SEVEN, KEY_EIGHT, KEY_NINE,
    KEY_SPACE, KEY_ESCAPE, KEY_ENTER, KEY_TAB, KEY_BACKSPACE, KEY_INSERT, KEY_DELETE, 
    KEY_RIGHT, KEY_LEFT, KEY_DOWN, KEY_UP,
    KEY_PAGE_UP, KEY_PAGE_DOWN, KEY_HOME, KEY_END,
    KEY_CAPS_LOCK, KEY_SCROLL_LOCK, KEY_NUM_LOCK, 
    KEY_PRINT_SCREEN, KEY_PAUSE,
    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
    KEY_LEFT_SHIFT, KEY_LEFT_CONTROL, KEY_LEFT_ALT, KEY_LEFT_SUPER,
    KEY_RIGHT_SHIFT, KEY_RIGHT_CONTROL, KEY_RIGHT_ALT, KEY_RIGHT_SUPER,
    KEY_KB_MENU,
    KEY_KP_0, KEY_KP_1, KEY_KP_2, KEY_KP_3, KEY_KP_4, KEY_KP_5, KEY_KP_6, KEY_KP_7, KEY_KP_8, KEY_KP_9,
    KEY_KP_DECIMAL, KEY_KP_DIVIDE, KEY_KP_MULTIPLY, KEY_KP_SUBTRACT, KEY_KP_ADD, KEY_KP_ENTER, KEY_KP_EQUAL,
    KEY_GRAVE, KEY_SLASH, KEY_BACKSLASH, KEY_COMMA, KEY_PERIOD, KEY_SEMICOLON, KEY_APOSTROPHE,
    KEY_LEFT_BRACKET, KEY_RIGHT_BRACKET, KEY_MINUS, KEY_EQUAL
};
// clang-format on

static int key_from_str(char *key) {
  for (int i = 0; i < KEYS_AMOUNT; i++) {
    if (strcmp(key, KEY_LITERALS[i]) == 0) {
      return KEY_IDS[i];
    }
  }
  return 0;
}

#define KEYBIND(keybind_field, keybind_name)                                                                           \
  {                                                                                                                    \
    cJSON *temp_key = cJSON_GetObjectItemCaseSensitive(json, keybind_name);                                            \
    if (cJSON_IsString(temp_key))                                                                                      \
      keybind_field = key_from_str(temp_key->valuestring);                                                             \
  }

static void keybinds_on_reload() {
  CONFIG_READ("keybinds", json, {
    KEYBIND(KEYBINDS.move_foreward_key, "move-foreward");
    KEYBIND(KEYBINDS.move_backward_key, "move-backward");
    KEYBIND(KEYBINDS.move_left_key, "move-left");
    KEYBIND(KEYBINDS.move_right_key, "move-right");
    KEYBIND(KEYBINDS.open_close_save_menu_key, "open-close-save-menu");
    KEYBIND(KEYBINDS.open_close_backpack_menu_key, "open-close-backpack-menu");
    KEYBIND(KEYBINDS.open_close_debug_menu_key, "open-close-debug-menu");
    KEYBIND(KEYBINDS.open_close_save_menu_key, "close-cur-menu");
    KEYBIND(KEYBINDS.toggle_hitbox_key, "toggle-hitbox");
    KEYBIND(KEYBINDS.reload_key, "reload");
    KEYBIND(KEYBINDS.zoom_in_key, "zoom-in");
    KEYBIND(KEYBINDS.zoom_out_key, "zoom-out");
  });
}

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
  keybinds_on_reload();
}
