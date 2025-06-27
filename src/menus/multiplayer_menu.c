#include "menu_includes.h"
#include <raylib.h>

static void multiplayer_menu_join() {}

static void multiplayer_menu_host() {
    game_set_menu(&CLIENT_GAME, MENU_HOST_SERVER);
}

static void multiplayer_menu_back() { game_set_menu(&CLIENT_GAME, MENU_START); }

void multiplayer_menu_render(UiRenderer *renderer, const ClientGame *game) {
  UI_SETUP({
      .positions = {UI_CENTER, UI_CENTER},
      .alignment = UI_VERTICAL,
      .padding = 24,
      .scale = 1,
      .font_scale = CONFIG.default_font_size,
  });

  int x_offset = 0;
  int y_offset = -2;

  RENDER_TEXT({.text = "Multiplayer"});
  RENDER_SPACING({.height = 40});
  RENDER_BUTTON({
      .message = "Join",
      .texture = BUTTON_TEXTURE,
      .selected_texture = BUTTON_SELECTED_TEXTURE,
      .on_click_func = button_click_simple(multiplayer_menu_join),
      .x_offset = x_offset,
      .y_offset = y_offset,
  });
  RENDER_BUTTON({
      .message = "Host",
      .texture = BUTTON_TEXTURE,
      .selected_texture = BUTTON_SELECTED_TEXTURE,
      .on_click_func = button_click_simple(multiplayer_menu_host),
      .x_offset = x_offset,
      .y_offset = y_offset,
  });
  RENDER_BUTTON({
      .message = "Back",
      .texture = BUTTON_TEXTURE,
      .selected_texture = BUTTON_SELECTED_TEXTURE,
      .on_click_func = button_click_simple(multiplayer_menu_back),
      .x_offset = x_offset,
      .y_offset = y_offset,
  });
}
