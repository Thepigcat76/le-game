#include "../../include/game.h"
#include "../../include/ui.h"
#include <raylib.h>
#include <stdlib.h>

static void start_menu_create_world() {
  game_set_menu(&GAME, MENU_NONE);
  GAME.paused = false;
}

static void start_menu_leave_game() {
  game_unload(&GAME);

  CloseWindow();
  CloseAudioDevice();
  exit(0);
}

void start_menu_render(UiRenderer *renderer, const Game *game) {
  ui_set_style(renderer, (UiStyle){
                             .positions = {UI_CENTER, UI_CENTER},
                             .alignment = UI_VERTICAL,
                             .padding = 24,
                         });
  ui_setup(renderer);

  int x_offset = 0;
  int y_offset = -2;

  ui_text_render(renderer, "LE GAME");
  ui_spacing_render(renderer, 100);
  ui_button_render_offset(renderer, "Create World", BUTTON_TEXTURE,
                          BUTTON_SELECTED_TEXTURE, start_menu_create_world,
                          vec2i(x_offset, y_offset));
  ui_button_render_offset(renderer, "Leave Game", BUTTON_TEXTURE,
                          BUTTON_SELECTED_TEXTURE, start_menu_leave_game,
                          vec2i(x_offset, y_offset));
}
