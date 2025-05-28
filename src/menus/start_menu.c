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
  ui_setup(renderer,
               (UiStyle){
                   .positions = {UI_CENTER, UI_CENTER},
                   .alignment = UI_VERTICAL,
                   .padding = 24,
                   .scale = 1,
                   .font_scale = CONFIG.default_font_size,
               });

  int x_offset = 0;
  int y_offset = -2;

  ui_text_render(renderer, "COZY WRATH");
  ui_spacing_render(renderer, 100);
  ui_button_render_offset(renderer, "New Save", BUTTON_TEXTURE, BUTTON_SELECTED_TEXTURE,
                          button_click_simple(start_menu_new_save), vec2i(x_offset, y_offset));
  ui_button_render_offset(renderer, "Load Save", BUTTON_TEXTURE, BUTTON_SELECTED_TEXTURE,
                          button_click_simple(start_menu_load_save), vec2i(x_offset, y_offset));
  ui_button_render_offset(renderer, "Settings", BUTTON_TEXTURE, BUTTON_SELECTED_TEXTURE,
                          button_click_simple(start_menu_settings), vec2i(x_offset, y_offset));
  ui_button_render_offset(renderer, "Leave Game", BUTTON_TEXTURE, BUTTON_SELECTED_TEXTURE,
                          button_click_simple(start_menu_leave_game), vec2i(x_offset, y_offset));
}
