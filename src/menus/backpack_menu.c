#include "../../include/game.h"
#include "../../include/ui.h"
#include "../../include/config.h"
#include <raylib.h>

void backpack_menu_render(UiRenderer *renderer, const Game *game) {
  ui_set_style(renderer, (UiStyle){
                             .positions = {UI_CENTER, UI_CENTER},
                             .alignment = UI_VERTICAL,
                             .padding = 24,
                             .scale = 1,
                             .font_scale = CONFIG.default_font_size,
                         });
  ui_setup(renderer);

  ui_set_background(renderer, BACKPACK_BACK_GROUND);

  ui_text_render_offset(renderer, "Backpack", vec2i(0, -60));
}
