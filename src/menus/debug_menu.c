#include "menu_includes.h"

static char text_buf[256];
static TextInputBuffer text_input_buffer = {.buf = text_buf, .len = 0, .max_len = 256};

static void debug_menu_items_button_clicked() {
  GAME.debug_options.game_object_display = (GAME.debug_options.game_object_display == DEBUG_DISPLAY_ALL_ITEMS)
      ? DEBUG_DISPLAY_NONE
      : DEBUG_DISPLAY_ALL_ITEMS;
}

static void debug_menu_tiles_button_clicked() {
  GAME.debug_options.game_object_display = (GAME.debug_options.game_object_display == DEBUG_DISPLAY_ALL_TILES)
      ? DEBUG_DISPLAY_NONE
      : DEBUG_DISPLAY_ALL_TILES;
}

static void debug_menu_beings_button_clicked() {
  GAME.debug_options.game_object_display = (GAME.debug_options.game_object_display == DEBUG_DISPLAY_ALL_BEINGS)
      ? DEBUG_DISPLAY_NONE
      : DEBUG_DISPLAY_ALL_BEINGS;
}

static void debug_menu_collisions_button_clicked() {
  GAME.debug_options.collisions_enabled = !GAME.debug_options.collisions_enabled;
}

static void debug_menu_hitboxes_button_clicked() {
  GAME.debug_options.hitboxes_shown = !GAME.debug_options.hitboxes_shown;
}

static void debug_menu_tile_info_button_clicked() {
  GAME.debug_options.print_tile_debug_info = !GAME.debug_options.print_tile_debug_info;
}

void debug_menu_render(UiRenderer *renderer, const ClientGame *game) {
  UI_SETUP({
               .positions = {UI_LEFT, UI_TOP},
               .alignment = UI_HORIZONTAL,
               .padding = 0,
               .scale = 0.75,
               .font_scale = CONFIG.default_font_size * 0.75,
           });

  int x_offset = 0;
  int y_offset = 0;//4 + 1 * ui_scale(renderer);

  RENDER_BUTTON({.message = "Items",
                 .texture = DEBUG_BUTTON_TEXTURE,
                 .selected_texture = DEBUG_BUTTON_SELECTED_TEXTURE,
                 .on_click_func = button_click_simple(debug_menu_items_button_clicked),
                 .x_offset = x_offset,
                 .y_offset = y_offset});
  RENDER_BUTTON({.message = "Tiles",
                 .texture = DEBUG_BUTTON_TEXTURE,
                 .selected_texture = DEBUG_BUTTON_SELECTED_TEXTURE,
                 .on_click_func = button_click_simple(debug_menu_tiles_button_clicked),
                 .x_offset = x_offset,
                 .y_offset = y_offset});
  RENDER_BUTTON({.message = "Beings",
                 .texture = DEBUG_BUTTON_TEXTURE,
                 .selected_texture = DEBUG_BUTTON_SELECTED_TEXTURE,
                 .on_click_func = button_click_simple(debug_menu_beings_button_clicked),
                 .x_offset = x_offset,
                 .y_offset = y_offset});
  RENDER_BUTTON({.message = GAME.debug_options.collisions_enabled ? "Collisions: On" : "Collisions: Off",
                 .texture = DEBUG_BUTTON_TEXTURE,
                 .selected_texture = DEBUG_BUTTON_SELECTED_TEXTURE,
                 .on_click_func = button_click_simple(debug_menu_collisions_button_clicked),
                 .x_offset = x_offset,
                 .y_offset = y_offset,
                 .width = 100,
                 .height = 16});
  RENDER_BUTTON({.message = GAME.debug_options.hitboxes_shown ? "Hitboxes: Shown" : "Hitboxes: Hidden",
                 .texture = DEBUG_BUTTON_TEXTURE,
                 .selected_texture = DEBUG_BUTTON_SELECTED_TEXTURE,
                 .on_click_func = button_click_simple(debug_menu_hitboxes_button_clicked),
                 .x_offset = x_offset,
                 .y_offset = y_offset,
                 .width = 120,
                 .height = 16});
  RENDER_BUTTON({.message = GAME.debug_options.print_tile_debug_info ? "Print tile info" : "Don't Print tile info",
                 .texture = DEBUG_BUTTON_TEXTURE,
                 .selected_texture = DEBUG_BUTTON_SELECTED_TEXTURE,
                 .on_click_func = button_click_simple(debug_menu_tile_info_button_clicked),
                 .x_offset = x_offset,
                 .y_offset = y_offset,
                 .width = 132,
                 .height = 16});
  RENDER_TEXT({.text = TextFormat("x: %d, y: %d", (int) game->player->box.x, (int) game->player->box.y), .color = WHITE});
}