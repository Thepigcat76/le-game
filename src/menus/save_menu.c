#include "menu_includes.h"
#include <pthread.h>
#include <raylib.h>

static void save_menu_back_to_game_button_clicked() {
  CLIENT_GAME.cur_menu = MENU_NONE;
  CLIENT_GAME.paused = false;
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
  packet_send(server_addr, PACKET_C2S_CLIENT_DISCONNECT_NEW({.player_id = CLIENT_GAME.player_id}), true);

  client_set_menu(&CLIENT_GAME, MENU_START);

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
/*
  RENDER_TEXT({.text = "Game Paused"});
  RENDER_BUTTON({.message = "Back To Game",
                 .texture = OPT_TEX(BACK_TO_GAME_BUTTON_TEXTURE),
                 .selected_texture = OPT_TEX(BACK_TO_GAME_BUTTON_TEXTURE_SELECTED),
                 .on_click_func = button_click_simple(save_menu_back_to_game_button_clicked),
                 .text_x_offset = x_offset,
                 .text_y_offset = y_offset});
  RENDER_BUTTON({.message = "General Settings",
                 .texture = OPT_TEX(VISUAL_SETTINGS_BUTTON_TEXTURE),
                 .selected_texture = OPT_TEX(VISUAL_SETTINGS_BUTTON_TEXTURE_SELECTED),
                 .on_click_func = button_click_simple(save_menu_general_settings_button_clicked),
                 .text_x_offset = x_offset,
                 .text_y_offset = y_offset});
  RENDER_BUTTON({.message = "Game Settings",
                 .texture = OPT_TEX(GAME_SETTINGS_BUTTON_TEXTURE),
                 .selected_texture = OPT_TEX(GAME_SETTINGS_BUTTON_TEXTURE_SELECTED),
                 .on_click_func = button_click_simple(save_menu_gameplay_settings_button_clicked),
                 .text_x_offset = x_offset,
                 .text_y_offset = y_offset});
  RENDER_BUTTON({.message = "Save Game",
                 .texture = OPT_TEX(LEAVE_GAME_BUTTON_TEXTURE),
                 .selected_texture = OPT_TEX(LEAVE_GAME_BUTTON_TEXTURE_SELECTED),
                 .on_click_func = button_click_simple(save_menu_save_game_button_clicked),
                 .text_x_offset = x_offset,
                 .text_y_offset = y_offset});
                 */
}
