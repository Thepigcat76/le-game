#include "../../include/config.h"
#include "../../include/game.h"
#include "../../include/ui.h"
#include <raylib.h>

static int scroll_y_offset = 0;

static void load_save_menu_load_save(void *args) {
  int save_index = *(int *)args;
  GAME.cur_save = save_index;
  game_load_cur_save(&GAME);
  game_cur_save_init(&GAME);
  world_initialize(&GAME.world);
  game_set_menu(&GAME, MENU_NONE);
  GAME.paused = false;
}

static void load_save_menu_back() { game_set_menu(&GAME, MENU_START); }

void load_save_menu_render(UiRenderer *renderer, const Game *game) {
  ui_setup(renderer,
           (UiStyle){
               .positions = {UI_CENTER, UI_CENTER},
               .alignment = UI_VERTICAL,
               .padding = 24,
               .scale = 1,
               .font_scale = CONFIG.default_font_size,
           });

  int x_offset = 0;
  int y_offset = 4;

  ui_text_render(renderer, "Load save");
  ui_spacing_render(renderer, 100);
  ui_text_render(renderer, TextFormat("Loaded Saves: %d", game->detected_saves));
  // Create the group for displaying the saves
  ui_group_create_dimensions(renderer, renderer->cur_style, true, 400, 300, &scroll_y_offset);
  for (int i = 0; i < game->detected_saves; i++) {
    ui_button_render_offset(renderer, TextFormat("%s", game->save_configs[i].save_name), SAVE_SLOT_TEXTURE,
                            SAVE_SLOT_SELECTED_TEXTURE, button_click_args(load_save_menu_load_save, &i),
                            vec2i(x_offset, y_offset));
  }
  ui_group_destroy(renderer);
  // End the group for displaying the saves
  ui_button_render_offset(renderer, "Back", BUTTON_TEXTURE, BUTTON_SELECTED_TEXTURE,
                          button_click_simple(load_save_menu_back), vec2i(x_offset, y_offset));
}
