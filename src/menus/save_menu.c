#include "../../include/menus.h"
#include "../../include/ui_layout.h"

void save_menu_layout_build(SaveMenu *menu) {
  Layout *layout = &menu->layout;
  layout_set_style(
      layout, (UiStyle){.padding = 100, .positions = {UI_CENTER, UI_CENTER}});
  layout_text_simple(layout, "Game Paused");
  layout_text_simple(layout, "Deez nuts");
  layout_text_simple(layout, "Ballz");
}
