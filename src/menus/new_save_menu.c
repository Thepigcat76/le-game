#include "../../include/config.h"
#include "../../include/game.h"
#include "../../include/ui.h"
#include <raylib.h>

static char _text_buf_0[256];
static TextInputBuffer save_name_text_input_buffer = {.buf = _text_buf_0, .len = 0, .max_len = 256};

static char _text_buf_1[256];
static TextInputBuffer seed_text_input_buffer = {.buf = _text_buf_1, .len = 0, .max_len = 256};

static void new_save_create_world() {
  float seed = string_to_world_seed(seed_text_input_buffer.buf);
  TraceLog(LOG_DEBUG, "World seed: %f", seed);
  game_set_menu(&GAME, MENU_NONE);
  GAME.paused = false;

  game_create_save(&GAME, "Test", seed);
  game_create_world(&GAME, seed);
  world_initialize(&GAME.world);
}

static void new_save_back_to_start_menu() { game_set_menu(&GAME, MENU_START); }

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
  ui_spacing_render(renderer, 40);
  ui_text_render(renderer, "Save Name:");
  ui_text_input_render(renderer, TEXT_INPUT_TEXTURE, &save_name_text_input_buffer);
  ui_text_render(renderer, "Seed:");
  ui_text_input_render(renderer, TEXT_INPUT_TEXTURE, &seed_text_input_buffer);
  ui_button_render_offset(renderer, "Create World", BUTTON_TEXTURE, BUTTON_SELECTED_TEXTURE,
                          button_click_simple(new_save_create_world), vec2i(x_offset, y_offset));
  ui_button_render_offset(renderer, "Back", BUTTON_TEXTURE, BUTTON_SELECTED_TEXTURE,
                          button_click_simple(new_save_back_to_start_menu), vec2i(x_offset, y_offset));
}
