#include "../../include/config.h"
#include "../../include/game.h"
#include "../../include/ui.h"
#include <raylib.h>

static char _text_buf[256];
static TextInputBuffer text_input_buffer = {.buf = _text_buf, .len = 0, .max_len = 256};

static void load_save_menu_load_save() {
  if (FileExists("save/game.bin")) {
    game_load(&GAME);
  }
  world_initialize(&GAME.world);
  game_set_menu(&GAME, MENU_NONE);
}

static void load_save_menu_back() { game_set_menu(&GAME, MENU_START); }

void load_save_menu_render(UiRenderer *renderer, const Game *game) {
  ui_set_style(renderer,
               (UiStyle){
                   .positions = {UI_CENTER, UI_CENTER},
                   .alignment = UI_VERTICAL,
                   .padding = 24,
                   .scale = 1,
                   .font_scale = CONFIG.default_font_size,
               });
  ui_setup(renderer);

  int x_offset = 0;
  int y_offset = 4;

  ui_text_render(renderer, "Load save");
  ui_spacing_render(renderer, 100);
  ui_text_render(renderer, TextFormat("Loaded Saves: %d", game->detected_saves));
  ui_button_render_offset(renderer, "Save #1", SAVE_SLOT_TEXTURE, SAVE_SLOT_TEXTURE,
                          button_click_simple(load_save_menu_load_save), vec2i(x_offset, y_offset));
  ui_button_render_offset(renderer, "Back", BUTTON_TEXTURE, BUTTON_SELECTED_TEXTURE,
                          button_click_simple(load_save_menu_back), vec2i(x_offset, y_offset));
}
