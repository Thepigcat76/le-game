#include "../../include/config.h"
#include "../../include/game.h"
#include "../../include/ui.h"
#include <raylib.h>
#include <stdlib.h>

static void start_menu_new_save() {
  game_detect_saves(&GAME);
  game_set_menu(&GAME, MENU_NEW_SAVE);
  StopMusicStream(MUSIC);
}

static void start_menu_load_save() {
  game_detect_saves(&GAME);
  game_set_menu(&GAME, MENU_LOAD_SAVE);
  StopMusicStream(MUSIC);
}

static void start_menu_settings() {}

static void start_menu_leave_game() {
  game_unload(&GAME);

  CloseAudioDevice();
  CloseWindow();
  exit(0);
}

void start_menu_render(UiRenderer *renderer, const Game *game) {
  UI_SETUP({
      .positions = {UI_CENTER, UI_CENTER},
      .alignment = UI_VERTICAL,
      .padding = 24,
      .scale = 1,
      .font_scale = CONFIG.default_font_size,
  });

  int x_offset = 0;
  int y_offset = -2;

  RENDER_TEXT({.text = "COZY WRATH"});
  RENDER_SPACING({.height = 100});
  RENDER_BUTTON({.message = "New Save",
                 .texture = BUTTON_TEXTURE,
                 .selected_texture = BUTTON_SELECTED_TEXTURE,
                 .on_click_func = button_click_simple(start_menu_new_save),
                 .x_offset = x_offset,
                 .y_offset = y_offset});
  RENDER_BUTTON({.message = "Load Save",
                 .texture = BUTTON_TEXTURE,
                 .selected_texture = BUTTON_SELECTED_TEXTURE,
                 .on_click_func = button_click_simple(start_menu_load_save),
                 .x_offset = x_offset,
                 .y_offset = y_offset});
  RENDER_BUTTON({.message = "Settings",
                 .texture = BUTTON_TEXTURE,
                 .selected_texture = BUTTON_SELECTED_TEXTURE,
                 .on_click_func = button_click_simple(start_menu_settings),
                 .x_offset = x_offset,
                 .y_offset = y_offset});
  RENDER_BUTTON({.message = "Leave Game",
                 .texture = BUTTON_TEXTURE,
                 .selected_texture = BUTTON_SELECTED_TEXTURE,
                 .on_click_func = button_click_simple(start_menu_leave_game),
                 .x_offset = x_offset,
                 .y_offset = y_offset});
}
