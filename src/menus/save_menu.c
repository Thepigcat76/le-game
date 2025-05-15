#include "../../include/game.h"
#include "../../include/ui.h"
#include "../../include/config.h"
#include <raylib.h>

static void save_menu_button_0_clicked(void *_, void *game) {
  TraceLog(LOG_INFO, "Button clicked");
}

void save_menu_render(UiRenderer *renderer, const Game *game) {
  ui_set_style(renderer, (UiStyle){
                             .positions = {UI_CENTER, UI_CENTER},
                             .alignment = UI_VERTICAL,
                             .padding = 24,
                         });
  ui_setup(renderer);

  int x_offset = 8 * (CONFIG.default_font_size / 10);
  int y_offset = -2;

  ui_text_render(renderer, "Game Paused");
  ui_button_render_offset(renderer, "Back To Game", BACK_TO_GAME_BUTTON_TEXTURE,
                          BUTTON_SELECTED_TEXTURE, save_menu_button_0_clicked,
                          vec2i(x_offset, y_offset));
  ui_button_render_offset(
      renderer, "General Settings", VISUAL_SETTINGS_BUTTON_TEXTURE,
      BUTTON_SELECTED_TEXTURE, save_menu_button_0_clicked, vec2i(x_offset, y_offset));
  ui_button_render_offset(renderer, "Gameplay Settings",
                          GAME_SETTINGS_BUTTON_TEXTURE, BUTTON_SELECTED_TEXTURE,
                          save_menu_button_0_clicked, vec2i(x_offset, y_offset));
  ui_button_render_offset(renderer, "Save Game", LEAVE_GAME_BUTTON_TEXTURE,
                          BUTTON_SELECTED_TEXTURE, save_menu_button_0_clicked,
                          vec2i(x_offset, y_offset));
}
