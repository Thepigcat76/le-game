#include "menu_includes.h"
#include <raylib.h>

void inventory_menu_render(UiRenderer *renderer, const ClientGame *game) {
  UI_SETUP({
      .positions = {UI_CENTER, UI_CENTER},
      .alignment = UI_VERTICAL,
      .padding = 24,
      .scale = 1,
      .font_scale = CONFIG.default_font_size,
  });

  int x_offset = 8 * (CONFIG.default_font_size / 10);
  int y_offset = -2;

  RENDER_TEXT({.text = "Inventory"});
  RENDER_SLOT({.item = &CLIENT_PLAYER->inv_container.items[0]});
  RENDER_SLOT({.item = &CLIENT_PLAYER->inv_container.items[1]});
  RENDER_SLOT({.item = &CLIENT_PLAYER->inv_container.items[2]});
  RENDER_SLOT({.item = &CLIENT_PLAYER->held_item});
  
}
