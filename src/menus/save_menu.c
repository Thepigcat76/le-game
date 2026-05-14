#include "../../include/net/payloads.h"
#include "menu_includes.h"
#include <pthread.h>
#include <raylib.h>

static void save_menu_back_to_game_button_clicked() {
  CLIENT_GAME.state.cur_menu = MENU_NONE;
  CLIENT_GAME.state.paused = false;
}

static void save_menu_general_settings_button_clicked() { TraceLog(LOG_DEBUG, "Button clicked"); }

static void save_menu_gameplay_settings_button_clicked() { TraceLog(LOG_DEBUG, "Button clicked"); }

static void save_menu_save_game_button_clicked() {
  addr_t server_addr;
  pthread_mutex_lock(&CLIENT_MUTEX);
  {
    server_addr = CLIENT_CONNECTION.server_addr;
  }
  pthread_mutex_unlock(&CLIENT_MUTEX);

  if (CLIENT_GAME.connected_to_server) {
    client_leave_server(&CLIENT_GAME);
  }

  client_set_menu(&CLIENT_GAME, MENU_START);

  CLIENT_GAME.game.save_loaded = false;

  game_unload_save(&CLIENT_GAME.game);
}

static Texture2D DECLARE_BUTTON_TEXTURE(BACK_TO_GAME_BUTTON_TEXTURE);
static Texture2D DECLARE_BUTTON_TEXTURE(VISUAL_SETTINGS_BUTTON_TEXTURE);
static Texture2D DECLARE_BUTTON_TEXTURE(GAME_SETTINGS_BUTTON_TEXTURE);
static Texture2D DECLARE_BUTTON_TEXTURE(LEAVE_GAME_BUTTON_TEXTURE);

void save_menu_init() {
  INIT_TEXTURE(BACK_TO_GAME_BUTTON_TEXTURE, "back_to_game_button");
  INIT_TEXTURE(VISUAL_SETTINGS_BUTTON_TEXTURE, "visual_settings_button");
  INIT_TEXTURE(GAME_SETTINGS_BUTTON_TEXTURE, "game_settings_button");
  INIT_TEXTURE(LEAVE_GAME_BUTTON_TEXTURE, "leave_game_button");
}

void save_menu_render(UiRenderer *renderer, const ClientGame *game) {
  UI_SETUP({
      .positions = {UI_CENTER, UI_CENTER},
      .alignment = UI_VERTICAL,
      .padding = 24,
      .scale = 1,
      .font_scale = CONFIG.default_font_size,
  });

  int x_offset = 8 * (CONFIG.default_font_size / 10);
  int y_offset = -2;

  RENDER_TEXT({.text = "Game Paused"});
  RENDER_BUTTON({
      .message = "Back To Game",
      .texture = OPT_TEX(TEX_BUTTON_BACK_TO_GAME),
      .selected_texture = OPT_TEX(TEX_BUTTON_SELECTED_BACK_TO_GAME),
      .on_click_func = button_click_simple(save_menu_back_to_game_button_clicked),
      .text_x_offset = x_offset,
      .text_y_offset = y_offset,
  });
  RENDER_BUTTON({
      .message = "Client Settings",
      .texture = OPT_TEX(TEX_BUTTON_CLIENT_SETTINGS),
      .selected_texture = OPT_TEX(TEX_BUTTON_SELECTED_CLIENT_SETTINGS),
      .on_click_func = button_click_simple(save_menu_gameplay_settings_button_clicked),
      .text_x_offset = x_offset,
      .text_y_offset = y_offset,
  });
  RENDER_BUTTON({
      .message = "Game Settings",
      .texture = OPT_TEX(TEX_BUTTON_GAME_SETTINGS),
      .selected_texture = OPT_TEX(TEX_BUTTON_SELECTED_GAME_SETTINGS),
      .on_click_func = button_click_simple(save_menu_general_settings_button_clicked),
      .text_x_offset = x_offset,
      .text_y_offset = y_offset,
  });
  RENDER_BUTTON({
      .message = "Save Game",
      .texture = OPT_TEX(TEX_BUTTON_LEAVE_GAME),
      .selected_texture = OPT_TEX(TEX_BUTTON_SELECTED_LEAVE_GAME),
      .on_click_func = button_click_simple(save_menu_save_game_button_clicked),
      .text_x_offset = x_offset,
      .text_y_offset = y_offset,
  });
}
