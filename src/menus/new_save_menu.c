#include "../../include/config.h"
#include "../../include/game.h"
#include "../../include/ui.h"
#include <raylib.h>
#include <stdlib.h>

static char _text_buf[256];
static TextInputBuffer text_input_buffer = {.buf = _text_buf, .len = 0, .max_len = 256};

static void new_save_create_world() {
  float seed = string_to_world_seed(text_input_buffer.buf);
  TraceLog(LOG_DEBUG, "World seed: %f", seed);
  game_set_menu(&GAME, MENU_NONE);
  GAME.paused = false;

  game_create_world(&GAME, seed);
  world_initialize(&GAME.world);
}

void new_save_menu_render(UiRenderer *renderer, const Game *game) {
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
  int y_offset = -2;

  ui_text_render(renderer, "Create new save");
  ui_spacing_render(renderer, 100);
  ui_text_input_render_dimensions(renderer, TEXT_INPUT_TEXTURE, &text_input_buffer, vec2i(128, 16));
  ui_button_render_offset(renderer, "Create World", BUTTON_TEXTURE, BUTTON_SELECTED_TEXTURE, button_click_simple(new_save_create_world),
                          vec2i(x_offset, y_offset));
}
