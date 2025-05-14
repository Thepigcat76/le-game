#include "../../include/game.h"
#include "../../include/ui.h"
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

  ui_text_render(renderer, "Game Paused");
  ui_button_render(renderer, "Back To Game", BUTTON_TEXTURE,
                   BUTTON_SELECTED_TEXTURE, save_menu_button_0_clicked);
  ui_button_render(renderer, "General Settings", BUTTON_TEXTURE,
                   BUTTON_SELECTED_TEXTURE, save_menu_button_0_clicked);
  ui_button_render(renderer, "Gameplay Settings", BUTTON_TEXTURE,
                   BUTTON_SELECTED_TEXTURE, save_menu_button_0_clicked);
  ui_button_render(renderer, "Save Game", BUTTON_TEXTURE,
                   BUTTON_SELECTED_TEXTURE, save_menu_button_0_clicked);
}
