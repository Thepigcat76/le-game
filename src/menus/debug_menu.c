#include "../../include/config.h"
#include "../../include/game.h"
#include "../../include/ui.h"
#include <raylib.h>

static void debug_menu_button_clicked() {}

void debug_menu_render(UiRenderer *renderer, const Game *game) {
  ui_set_style(renderer,
               (UiStyle){
                   .positions = {UI_LEFT, UI_TOP},
                   .alignment = UI_HORIZONTAL,
                   .padding = 0,
               });
  ui_setup(renderer);

  int x_offset = 0;
  int y_offset = 4 + 1 * ui_scale(renderer);

  ui_button_render_offset(renderer, "Test 1", DEBUG_BUTTON_TEXTURE, DEBUG_BUTTON_SELECTED_TEXTURE,
                          debug_menu_button_clicked, vec2i(x_offset, y_offset));
  ui_button_render_offset(renderer, "Test 2", DEBUG_BUTTON_TEXTURE, DEBUG_BUTTON_SELECTED_TEXTURE,
                          debug_menu_button_clicked, vec2i(x_offset, y_offset));
}