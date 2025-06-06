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

  RENDER_TEXT({.text = "Load save"});
  RENDER_SPACING({.height = 100});
  RENDER_TEXT({.text = TextFormat("Loaded Saves: %d", game->detected_saves)});
  // Create the group for displaying the saves
UI_GROUP_CREATE({.group_style = renderer->cur_style,
                .has_scrollbar = true,
                .width = 400,
                .height = 300,
                .scroll_y_offset = &scroll_y_offset});
  {
    for (int i = 0; i < game->detected_saves; i++) {
      RENDER_BUTTON({.message = TextFormat("%s", game->save_configs[i].save_name),
                     .texture = SAVE_SLOT_TEXTURE,
                     .selected_texture = SAVE_SLOT_SELECTED_TEXTURE,
                     .on_click_func = button_click_args(load_save_menu_load_save, &i),
                     .x_offset = x_offset,
                     .y_offset = y_offset});
    }
  }
  UI_GROUP_DESTROY();
  // End the group for displaying the saves
  RENDER_BUTTON({.message = "Back",
                 .texture = BUTTON_TEXTURE,
                 .selected_texture = BUTTON_SELECTED_TEXTURE,
                 .on_click_func = button_click_simple(load_save_menu_back),
                 .x_offset = x_offset,
                 .y_offset = y_offset});
}
