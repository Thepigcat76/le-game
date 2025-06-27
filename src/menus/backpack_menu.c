#include "menu_includes.h"

void backpack_menu_render(UiRenderer *renderer, const ClientGame *game) {
  ui_setup(renderer,
           (UiStyle){
               .positions = {UI_CENTER, UI_CENTER},
               .alignment = UI_VERTICAL,
               .padding = 24,
               .scale = 1,
               .font_scale = CONFIG.default_font_size,
           });

  UI_BACKGROUND({.texture = BACKPACK_BACK_GROUND});

  RENDER_TEXT({.text = "Backpack", .x_offset = 0, .y_offset = -60, .color = WHITE});
}
