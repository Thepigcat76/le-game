#include "../../include/save_desc.h"
#include "lilc/array.h"
#include "menu_includes.h"
#include <lilc/alloc.h>
#include <string.h>

static bool save_name_input_selected = true;
static bool seed_input_selected = false;

static char _text_buf_0[256];
static TextInputBuffer save_name_text_input_buffer = {.buf = _text_buf_0, .len = 0, .max_len = 256};

static char _text_buf_1[256];
static TextInputBuffer seed_text_input_buffer = {.buf = _text_buf_1, .len = 0, .max_len = 256};

static void new_save_create_world() {
  client_set_menu(&CLIENT_GAME, MENU_NONE);
  CLIENT_GAME.paused = false;

  SaveDescriptor desc = {
      .id = array_len(CLIENT_GAME.local_saves),
      .config = {.save_name = save_name_text_input_buffer.buf, .seed = string_to_world_seed(seed_text_input_buffer.buf)}};
  game_create_save(&CLIENT_GAME.game, desc);
  game_create_save_world(&CLIENT_GAME.game);
  client_init_loaded_save(&CLIENT_GAME, &CLIENT_GAME.game.cur_save);
}

static void new_save_back_to_start_menu() { client_set_menu(&CLIENT_GAME, MENU_START); }

void new_save_menu_open(UiRenderer *renderer, const ClientGame *game) {
  dyn_string_t random_save_name = generate_save_name(&HEAP_ALLOCATOR);
  int len = random_save_name.len;
  strcpy(save_name_text_input_buffer.buf, random_save_name.string);
  save_name_text_input_buffer.len = len;
  dyn_string_free(&random_save_name);
}

void new_save_menu_render(UiRenderer *renderer, const ClientGame *game) {
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
  RENDER_TEXT_INPUT({
      .text_input = &save_name_text_input_buffer,
      .selected = &save_name_input_selected,
      .width = 200,
  });
  RENDER_TEXT({.text = "Seed:"});
  RENDER_TEXT_INPUT({
      .text_input = &seed_text_input_buffer,
      .selected = &seed_input_selected,
      .width = 200,
  });
  RENDER_BUTTON({
      .message = "Create World",
      .on_click_func = button_click_simple(new_save_create_world),
      .x_offset = x_offset,
      .y_offset = y_offset,
  });
  RENDER_BUTTON({
      .message = "Back",
      .on_click_func = button_click_simple(new_save_back_to_start_menu),
      .x_offset = x_offset,
      .y_offset = y_offset,
  });
}
