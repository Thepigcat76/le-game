#include "../../include/game.h"

#define INIT_MENU(menu_name)                                                                                           \
  extern void menu_name##_init();                                                                                      \
  menu_name##_init();

#define OPEN_MENU(ui_renderer, menu_name)                                                                              \
  extern void menu_name##_open(UiRenderer *renderer, const Game *game);                                                \
  menu_name##_open(ui_renderer, game);

void game_init_menu(Game *game) {
  INIT_MENU(save_menu);
  INIT_MENU(dialog_menu);
  // INIT_MENU(start_menu);
  // INIT_MENU(debug_menu);
}

bool game_cur_menu_hides_game(Game *game) {
  return game->cur_menu == MENU_START || game->cur_menu == MENU_NEW_SAVE || game->cur_menu == MENU_LOAD_SAVE ||
      game->cur_menu == MENU_MULTIPLAYER || game->cur_menu == MENU_HOST_SERVER;
}

static void game_open_menu(Game *game, MenuId menu_id) {
  switch (menu_id) {
  case MENU_NEW_SAVE: {
    OPEN_MENU(&GAME.ui_renderer, new_save_menu);
    break;
  }
  case MENU_HOST_SERVER: {
    OPEN_MENU(&GAME.ui_renderer, host_menu);
    break;
  }
  default: {
    break;
  }
  }
}

static void game_calc_ui_height(UiRenderer *ui_renderer) {
  if (ui_renderer->ui_height == -1) {
    ui_renderer->cur_y = 0;
    ui_renderer->simulate = true;
    game_render_menu(&GAME);
    ui_renderer->ui_height = ui_renderer->cur_y;

    ui_renderer->simulate = false;
    ui_renderer->cur_x = 0;
    ui_renderer->cur_y = 0;
  }
}

void game_set_menu(Game *game, MenuId menu_id) {
  game->cur_menu = menu_id;
  game_calc_ui_height(&game->ui_renderer);
  game_open_menu(game, menu_id);
}