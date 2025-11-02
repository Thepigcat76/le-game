#include "menu_includes.h"

static char text_buf[256];
static TextInputBuffer text_input_buffer = {.buf = text_buf, .len = 0, .max_len = 256};

static void debug_menu_items_button_clicked(void) {
  DebugOptions *opts = &CLIENT_GAME.game.debug.options;
  opts->game_object_display = (opts->game_object_display == DEBUG_DISPLAY_ALL_ITEMS) ? DEBUG_DISPLAY_NONE : DEBUG_DISPLAY_ALL_ITEMS;
}

static void debug_menu_tiles_button_clicked(void) {
  DebugOptions *opts = &CLIENT_GAME.game.debug.options;
  opts->game_object_display = (opts->game_object_display == DEBUG_DISPLAY_ALL_TILES) ? DEBUG_DISPLAY_NONE : DEBUG_DISPLAY_ALL_TILES;
}

static void debug_menu_beings_button_clicked(void) {
  DebugOptions *opts = &CLIENT_GAME.game.debug.options;
  opts->game_object_display = (opts->game_object_display == DEBUG_DISPLAY_ALL_BEINGS) ? DEBUG_DISPLAY_NONE : DEBUG_DISPLAY_ALL_BEINGS;
}

static void debug_menu_collisions_button_clicked(void) {
  DebugOptions *opts = &CLIENT_GAME.game.debug.options;
  opts->collisions_enabled = !opts->collisions_enabled;
}

static void debug_menu_hitboxes_button_clicked(void) {
  DebugOptions *opts = &CLIENT_GAME.game.debug.options;
  opts->hitboxes_shown = !opts->hitboxes_shown;
}

static void debug_menu_tile_info_button_clicked(void) {
  DebugOptions *opts = &CLIENT_GAME.game.debug.options;
  opts->print_tile_debug_info = !opts->print_tile_debug_info;
}

static void debug_menu_rerender_world(void) { world_prepare_rendering(CLIENT_GAME.world); }

void debug_menu_render(UiRenderer *renderer, const ClientGame *game) {
  UI_SETUP({
      .positions = {UI_LEFT, UI_TOP},
      .alignment = UI_HORIZONTAL,
      .padding = 0,
      .scale = 0.75,
      .font_scale = CONFIG.default_font_size * 0.75,
  });

  int x_offset = 0;
  int y_offset = 0; // 4 + 1 * ui_scale(renderer);

  DebugOptions *opts = &CLIENT_GAME.game.debug.options;

  char *items_msg = "Items";
  RENDER_BUTTON({.message = items_msg,
                 .on_click_func = button_click_simple(debug_menu_items_button_clicked),
                 .x_offset = x_offset,
                 .y_offset = y_offset,
                 .width = MeasureText(items_msg, UI_RENDERER_PTR->cur_style.font_scale)});
  char *tiles_msg = "Tiles";
  RENDER_BUTTON({.message = tiles_msg,
                 .on_click_func = button_click_simple(debug_menu_tiles_button_clicked),
                 .x_offset = x_offset,
                 .y_offset = y_offset,
                 .width = MeasureText(tiles_msg, UI_RENDERER_PTR->cur_style.font_scale)});
  char *beings_msg = "Beings";
  RENDER_BUTTON({.message = beings_msg,
                 .on_click_func = button_click_simple(debug_menu_beings_button_clicked),
                 .x_offset = x_offset,
                 .y_offset = y_offset,
                 .width = MeasureText(beings_msg, UI_RENDERER_PTR->cur_style.font_scale)});
  char *collisions_msg = opts->collisions_enabled ? "Collisions: On" : "Collisions: Off";
  RENDER_BUTTON({.message = collisions_msg,
                 .on_click_func = button_click_simple(debug_menu_collisions_button_clicked),
                 .x_offset = x_offset,
                 .y_offset = y_offset,
                 .width = MeasureText(collisions_msg, UI_RENDERER_PTR->cur_style.font_scale)});
  char *hitboxes_msg = opts->hitboxes_shown ? "Hitboxes: Shown" : "Hitboxes: Hidden";
  RENDER_BUTTON({.message = hitboxes_msg,
                 .on_click_func = button_click_simple(debug_menu_hitboxes_button_clicked),
                 .x_offset = x_offset,
                 .y_offset = y_offset,
                 .width = MeasureText(hitboxes_msg, UI_RENDERER_PTR->cur_style.font_scale)});
  char *tile_info_msg = opts->print_tile_debug_info ? "Print tile info" : "Don't Print tile info";
  RENDER_BUTTON({.message = tile_info_msg,
                 .on_click_func = button_click_simple(debug_menu_tile_info_button_clicked),
                 .x_offset = x_offset,
                 .y_offset = y_offset,
                 .width = MeasureText(tile_info_msg, UI_RENDERER_PTR->cur_style.font_scale)});
  char *rerender_world_msg = "Rerender world";
  RENDER_BUTTON({.message = rerender_world_msg,
                 .on_click_func = button_click_simple(debug_menu_rerender_world),
                 .width = MeasureText(rerender_world_msg, UI_RENDERER_PTR->cur_style.font_scale)});
  RENDER_TEXT({.text = TextFormat("x: %d, y: %d", (int)CLIENT_PLAYER->box.x, (int)CLIENT_PLAYER->box.y), .color = WHITE});
}
