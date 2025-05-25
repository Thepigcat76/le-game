#include "../../include/config.h"
#include "../../include/game.h"
#include "../../include/ui.h"
#include <raylib.h>

static char text_buf[256];
static TextInputBuffer text_input_buffer = {.buf = text_buf, .len = 0, .max_len = 256};

static void debug_menu_items_button_clicked() {
  GAME.debug_options.game_object_display =
      (GAME.debug_options.game_object_display == DISPLAY_ALL_ITEMS) ? DISPLAY_NONE : DISPLAY_ALL_ITEMS;
}
static void debug_menu_tiles_button_clicked() {
  GAME.debug_options.game_object_display =
      (GAME.debug_options.game_object_display == DISPLAY_ALL_TILES) ? DISPLAY_NONE : DISPLAY_ALL_TILES;
}
static void debug_menu_collisions_button_clicked() {
  GAME.debug_options.collisions_enabled = !GAME.debug_options.collisions_enabled;
}
static void debug_menu_hitboxes_button_clicked() {
  TraceLog(LOG_DEBUG, "Deez nuts");
  GAME.debug_options.hitboxes_shown = !GAME.debug_options.hitboxes_shown;
}

void debug_menu_render(UiRenderer *renderer, const Game *game) {
  ui_set_style(renderer,
               (UiStyle){
                   .positions = {UI_LEFT, UI_TOP},
                   .alignment = UI_HORIZONTAL,
                   .padding = 0,
                   .scale = 0.75,
                   .font_scale = CONFIG.default_font_size * 0.75,
               });
  ui_setup(renderer);

  int x_offset = 0;
  int y_offset = 4 + 1 * ui_scale(renderer);

  ui_button_render_offset(renderer, "Items", DEBUG_BUTTON_TEXTURE, DEBUG_BUTTON_SELECTED_TEXTURE,
                          button_click_simple(debug_menu_items_button_clicked), vec2i(x_offset, y_offset));
  ui_button_render_offset(renderer, "Tiles", DEBUG_BUTTON_TEXTURE, DEBUG_BUTTON_SELECTED_TEXTURE,
                          button_click_simple(debug_menu_tiles_button_clicked), vec2i(x_offset, y_offset));
  ui_button_render_dimensions_offset(
      renderer, GAME.debug_options.collisions_enabled ? "Collisions: On" : "Collisions: Off", DEBUG_BUTTON_TEXTURE,
      DEBUG_BUTTON_SELECTED_TEXTURE, button_click_simple(debug_menu_collisions_button_clicked),
      vec2i(x_offset, y_offset), vec2i(100, 16));
  ui_button_render_dimensions_offset(
      renderer, GAME.debug_options.hitboxes_shown ? "Hitboxes: Shown" : "Hitboxes: Hidden", DEBUG_BUTTON_TEXTURE,
      DEBUG_BUTTON_SELECTED_TEXTURE, button_click_simple(debug_menu_hitboxes_button_clicked), vec2i(x_offset, y_offset),
      vec2i(120, 16));
  ui_text_input_render_dimensions(renderer, TEXT_INPUT_TEXTURE, &text_input_buffer, vec2i(128, 16));
}