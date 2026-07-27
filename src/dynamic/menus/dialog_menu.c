#include "menu_includes.h"

static Texture2D TEXTURE;

void dialog_menu_open(ClientGame *client) {
    TEXTURE = LoadTexture("res/assets/gui/dialog_ui.png");
}

void dialog_menu_render(UiRenderer *renderer, ClientGame *game) {
  UI_SETUP({
      .positions = {UI_CENTER, UI_BOTTOM},
      .alignment = UI_HORIZONTAL,
      .padding = 0,
      .scale = 1,
      .font_scale = CONFIG.default_font_size,
  });

  UI_BACKGROUND({.texture = TEXTURE, .y_offset = -5 * 4.5});

  //RENDER_TEXTURE({.texture = });
}
