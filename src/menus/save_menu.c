#include "../../include/config.h"
#include "../../include/game.h"
#include "../../include/ui.h"
#include <raylib.h>

static void save_menu_back_to_game_button_clicked() {
  GAME.cur_menu = MENU_NONE;
  GAME.paused = false;
}

static void save_menu_general_settings_button_clicked() {
  TraceLog(LOG_DEBUG, "Button clicked");
}

static void save_menu_gameplay_settings_button_clicked() {
  TraceLog(LOG_DEBUG, "Button clicked");
}

static void save_menu_save_game_button_clicked() {
  GAME.cur_menu = MENU_START;
}

void save_menu_render(UiRenderer *renderer, const Game *game) {
  ui_set_style(renderer, (UiStyle){
                             .positions = {UI_CENTER, UI_CENTER},
                             .alignment = UI_VERTICAL,
                             .padding = 24,
                             .scale = 1,
                             .font_scale = CONFIG.default_font_size,
                         });
  ui_setup(renderer);

  int x_offset = 8 * (CONFIG.default_font_size / 10);
  int y_offset = -2;

  ui_text_render(renderer, "Game Paused");
  ui_button_render_offset(renderer, "Back To Game", BACK_TO_GAME_BUTTON_TEXTURE,
                          BACK_TO_GAME_SELECTED_BUTTON_TEXTURE,
                          save_menu_back_to_game_button_clicked,
                          vec2i(x_offset, y_offset));
  ui_button_render_offset(
      renderer, "General Settings", VISUAL_SETTINGS_BUTTON_TEXTURE,
      VISUAL_SETTINGS_SELECTED_BUTTON_TEXTURE,
      save_menu_general_settings_button_clicked, vec2i(x_offset, y_offset));
  ui_button_render_offset(
      renderer, "Gameplay Settings", GAME_SETTINGS_BUTTON_TEXTURE,
      GAME_SETTINGS_SELECTED_BUTTON_TEXTURE,
      save_menu_gameplay_settings_button_clicked, vec2i(x_offset, y_offset));
  ui_button_render_offset(renderer, "Save Game", LEAVE_GAME_BUTTON_TEXTURE,
                          LEAVE_GAME_SELECTED_BUTTON_TEXTURE,
                          save_menu_save_game_button_clicked,
                          vec2i(x_offset, y_offset));
}
