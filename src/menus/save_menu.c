#include "../../include/config.h"
#include "../../include/game.h"
#include "../../include/ui.h"
#include "../../include/ui/ui_helper.h"
#include <raylib.h>

static void save_menu_back_to_game_button_clicked() {
  GAME.cur_menu = MENU_NONE;
  GAME.paused = false;
}

static void save_menu_general_settings_button_clicked() { TraceLog(LOG_DEBUG, "Button clicked"); }

static void save_menu_gameplay_settings_button_clicked() { TraceLog(LOG_DEBUG, "Button clicked"); }

static void save_menu_save_game_button_clicked() {
  game_set_menu(&GAME, MENU_START);
  game_save_cur_save(&GAME);
  Chunk *chunks = GAME.world.chunks;
  GAME.world = world_new();
  GAME.world.chunks = chunks;
}

static Texture2D DECLARE_BUTTON_TEXTURE(BACK_TO_GAME_BUTTON_TEXTURE);
static Texture2D DECLARE_BUTTON_TEXTURE(VISUAL_SETTINGS_BUTTON_TEXTURE);
static Texture2D DECLARE_BUTTON_TEXTURE(GAME_SETTINGS_BUTTON_TEXTURE);
static Texture2D DECLARE_BUTTON_TEXTURE(LEAVE_GAME_BUTTON_TEXTURE);

void save_menu_init() {
  INIT_TEXTURE(BACK_TO_GAME_BUTTON_TEXTURE, "back_to_game_button");
  INIT_TEXTURE(VISUAL_SETTINGS_BUTTON_TEXTURE, "visual_settings_button");
  INIT_TEXTURE(GAME_SETTINGS_BUTTON_TEXTURE, "back_to_game_button");
  INIT_TEXTURE(LEAVE_GAME_BUTTON_TEXTURE, "back_to_game_button");
}

void save_menu_render(UiRenderer *renderer, const Game *game) {
  ui_setup(renderer,
               (UiStyle){
                   .positions = {UI_CENTER, UI_CENTER},
                   .alignment = UI_VERTICAL,
                   .padding = 24,
                   .scale = 1,
                   .font_scale = CONFIG.default_font_size,
               });

  int x_offset = 8 * (CONFIG.default_font_size / 10);
  int y_offset = -2;

  ui_text_render(renderer, "Game Paused");
  ui_button_render_offset(renderer, "Back To Game", BACK_TO_GAME_BUTTON_TEXTURE, BACK_TO_GAME_BUTTON_TEXTURE_SELECTED,
                          button_click_simple(save_menu_back_to_game_button_clicked), vec2i(x_offset, y_offset));
  ui_button_render_offset(renderer, "General Settings", VISUAL_SETTINGS_BUTTON_TEXTURE,
                          VISUAL_SETTINGS_BUTTON_TEXTURE_SELECTED,
                          button_click_simple(save_menu_general_settings_button_clicked), vec2i(x_offset, y_offset));
  ui_button_render_offset(renderer, "Gameplay Settings", GAME_SETTINGS_BUTTON_TEXTURE,
                          GAME_SETTINGS_BUTTON_TEXTURE_SELECTED,
                          button_click_simple(save_menu_gameplay_settings_button_clicked), vec2i(x_offset, y_offset));
  ui_button_render_offset(renderer, "Save Game", LEAVE_GAME_BUTTON_TEXTURE, LEAVE_GAME_BUTTON_TEXTURE_SELECTED,
                          button_click_simple(save_menu_save_game_button_clicked), vec2i(x_offset, y_offset));
}
