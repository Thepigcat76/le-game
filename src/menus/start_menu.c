#include "menu_includes.h"
#include <raylib.h>

static void start_menu_new_save(void) {
  game_load_saves(&CLIENT_GAME.game);
  client_set_menu(&CLIENT_GAME, MENU_NEW_SAVE);
  //StopMusicStream(MUSIC);
}

static void start_menu_load_save(void) {
  game_load_saves(&CLIENT_GAME.game);
  client_set_menu(&CLIENT_GAME, MENU_LOAD_SAVE);
  //StopMusicStream(MUSIC);
}

static void start_menu_multiplayer(void) {
  client_set_menu(&CLIENT_GAME, MENU_MULTIPLAYER);
}

static void start_menu_settings(void) {}

static void start_menu_leave_game(void) {
  client_stop_running(&CLIENT_GAME);
}

void start_menu_render(UiRenderer *renderer, const ClientGame *game) {
  UI_SETUP({
      .positions = {UI_CENTER, UI_CENTER},
      .alignment = UI_VERTICAL,
      .padding = 24,
      .scale = 1,
      .font_scale = CONFIG.default_font_size,
  });

  int x_offset = 0;
  int y_offset = -2;

  RENDER_TEXT({.text = "COZY WRATH"});
  RENDER_SPACING({.height = 100});
  RENDER_BUTTON({
      .message = "New Save",
      .on_click_func = button_click_simple(start_menu_new_save),
      .x_offset = x_offset,
      .y_offset = y_offset,
  });
  RENDER_BUTTON({
      .message = "Load Save",
      .on_click_func = button_click_simple(start_menu_load_save),
      .x_offset = x_offset,
      .y_offset = y_offset,
  });
  RENDER_BUTTON({
      .message = "Multiplayer",
      .on_click_func = button_click_simple(start_menu_multiplayer),
      .x_offset = x_offset,
      .y_offset = y_offset,
  });
  RENDER_BUTTON({
      .message = "Settings",
      .on_click_func = button_click_simple(start_menu_settings),
      .x_offset = x_offset,
      .y_offset = y_offset,
  });
  RENDER_BUTTON({
      .message = "Leave Game",
      .on_click_func = button_click_simple(start_menu_leave_game),
      .x_offset = x_offset,
      .y_offset = y_offset,
  });
}
