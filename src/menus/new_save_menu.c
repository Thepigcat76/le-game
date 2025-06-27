#include "../../include/config.h"
#include "../../include/game.h"
#include "../../include/save_names.h"
#include "../../include/ui.h"
#include <string.h>

static bool save_name_input_selected = true;
static bool seed_input_selected = false;

static char _text_buf_0[256];
static TextInputBuffer save_name_text_input_buffer = {.buf = _text_buf_0, .len = 0, .max_len = 256};

static char _text_buf_1[256];
static TextInputBuffer seed_text_input_buffer = {.buf = _text_buf_1, .len = 0, .max_len = 256};

static void new_save_create_world() {
  game_set_menu(&GAME, MENU_NONE);
  GAME.paused = false;

  game_create_save(&GAME, save_name_text_input_buffer.buf, seed_text_input_buffer.buf);
  world_initialize(GAME.world);
}

static void new_save_back_to_start_menu() { game_set_menu(&GAME, MENU_START); }

void new_save_menu_open(UiRenderer *renderer, const Game *game) {
  char *random_save_name = save_names_random_name();
  int len = strlen(random_save_name);
  strcpy(save_name_text_input_buffer.buf, random_save_name);
  save_name_text_input_buffer.len = len;
  free(random_save_name);
}

void new_save_menu_render(UiRenderer *renderer, const Game *game) {
  float scale = 0.85;
  UI_SETUP({
      .positions = {UI_CENTER, UI_CENTER},
      .alignment = UI_VERTICAL,
      .padding = 12,
      .scale = scale,
      .font_scale = CONFIG.default_font_size * scale,
  });

  int x_offset = 0;
  int y_offset = -2;

  RENDER_TEXT({.text = "Create new save"});
  RENDER_SPACING({.height = 40});
  RENDER_TEXT({.text = "Save Name:"});
  RENDER_TEXT_INPUT({.texture = TEXT_INPUT_TEXTURE,
                     .text_input = &save_name_text_input_buffer,
                     .selected = &save_name_input_selected,
                     .width = 200});
  RENDER_TEXT({.text = "Seed:"});
  RENDER_TEXT_INPUT({.texture = TEXT_INPUT_TEXTURE,
                     .text_input = &seed_text_input_buffer,
                     .selected = &seed_input_selected,
                     .width = 200});
  RENDER_BUTTON({.message = "Create World",
                 .texture = BUTTON_TEXTURE,
                 .selected_texture = BUTTON_SELECTED_TEXTURE,
                 .on_click_func = button_click_simple(new_save_create_world),
                 .x_offset = x_offset,
                 .y_offset = y_offset});
  RENDER_BUTTON({.message = "Back",
                 .texture = BUTTON_TEXTURE,
                 .selected_texture = BUTTON_SELECTED_TEXTURE,
                 .on_click_func = button_click_simple(new_save_back_to_start_menu),
                 .x_offset = x_offset,
                 .y_offset = y_offset});
}
