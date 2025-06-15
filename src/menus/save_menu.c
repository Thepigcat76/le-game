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
  GAME.world = world_new_no_chunks();
  GAME.world.chunks = chunks;
  GAME.feature_store.game_features_amount = 0;
  bump_reset(&ITEM_CONTAINER_BUMP);
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
  UI_SETUP({
      .positions = {UI_CENTER, UI_CENTER},
      .alignment = UI_VERTICAL,
      .padding = 24,
      .scale = 1,
      .font_scale = CONFIG.default_font_size,
  });

  int x_offset = 8 * (CONFIG.default_font_size / 10);
  int y_offset = -2;

  RENDER_TEXT({.text = "Game Paused"});
  RENDER_BUTTON({.message = "Back To Game",
                 .texture = BACK_TO_GAME_BUTTON_TEXTURE,
                 .selected_texture = BACK_TO_GAME_BUTTON_TEXTURE_SELECTED,
                 .on_click_func = button_click_simple(save_menu_back_to_game_button_clicked),
                 .text_x_offset = x_offset,
                 .text_y_offset = y_offset});
  RENDER_BUTTON({.message = "General Settings",
                 .texture = VISUAL_SETTINGS_BUTTON_TEXTURE,
                 .selected_texture = VISUAL_SETTINGS_BUTTON_TEXTURE_SELECTED,
                 .on_click_func = button_click_simple(save_menu_general_settings_button_clicked),
                 .text_x_offset = x_offset,
                 .text_y_offset = y_offset});
  RENDER_BUTTON({.message = "Game Settings",
                 .texture = GAME_SETTINGS_BUTTON_TEXTURE,
                 .selected_texture = GAME_SETTINGS_BUTTON_TEXTURE_SELECTED,
                 .on_click_func = button_click_simple(save_menu_gameplay_settings_button_clicked),
                 .text_x_offset = x_offset,
                 .text_y_offset = y_offset});
  RENDER_BUTTON({.message = "Save Game",
                 .texture = LEAVE_GAME_BUTTON_TEXTURE,
                 .selected_texture = LEAVE_GAME_BUTTON_TEXTURE_SELECTED,
                 .on_click_func = button_click_simple(save_menu_save_game_button_clicked),
                 .text_x_offset = x_offset,
                 .text_y_offset = y_offset});
}
