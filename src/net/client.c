#include "../../include/net/client.h"
#include "../../include/array.h"
#include <raylib.h>

#define RELOAD(client_game_ptr, src_file_prefix)                                                                              \
  extern void src_file_prefix##_on_reload(ClientGame *game);                                                                 \
  src_file_prefix##_on_reload(client_game_ptr);

ClientGame CLIENT_GAME;

static Music MUSIC;

static Bump SOUND_BUMP;
BUMP_ALLOCATOR(SOUND_BUMP_ALLOCATOR, &SOUND_BUMP);

void client_init(void) {
  CLIENT_GAME = (ClientGame){
      .cur_menu = MENU_START,
      .paused = false,
      .world_texture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight()),
      .window = {.prev_width = GetScreenWidth(),
                 .prev_height = GetScreenHeight(),
                 .width = GetScreenWidth(),
                 .height = GetScreenHeight()},
      .ui_renderer = (UiRenderer){.cur_x = 0,
                                  .cur_y = 0,
                                  .simulate = false,
                                  .ui_height = -1,
                                  .cur_style = {0},
                                  .initial_style = {0},
                                  .context = {.screen_width = GetScreenWidth(), .screen_height = GetScreenHeight()}}};

  ClientGame *game = &CLIENT_GAME;

  game_init_menu(game);

  bump_init(&SOUND_BUMP, malloc(sizeof(Sound) * 1000), sizeof(Sound) * 1000);

  game->sound_manager.sound_buffers[SOUND_PLACE].base_sound = LoadSound("res/sounds/place_sound.wav");
  game->sound_manager.sound_buffers[SOUND_PLACE].sound_buf =
      array_new_capacity(Sound, SOUND_BUFFER_LIMIT, &SOUND_BUMP_ALLOCATOR);

  for (int i = 0; i < SOUND_BUFFER_LIMIT; i++) {
    game->sound_manager.sound_buffers[SOUND_PLACE].sound_buf[i] =
        LoadSoundAlias(game->sound_manager.sound_buffers[SOUND_PLACE].base_sound);
    SetSoundPitch(game->sound_manager.sound_buffers[SOUND_PLACE].sound_buf[i], 0.5);
    SetSoundVolume(game->sound_manager.sound_buffers[SOUND_PLACE].sound_buf[i], 0.25);
  }

  MUSIC = LoadMusicStream("res/music/main_menu_music.ogg");
  SetMusicVolume(MUSIC, 0.15);
  SetMusicPitch(MUSIC, 0.85);
  // PlayMusicStream(MUSIC);
}

void game_client_reload(ClientGame *game) {
  RELOAD(game, tile);
  RELOAD(game, keybinds);
  RELOAD(game, save_names);
  RELOAD(game, shaders);
}

// MENUS

#define INIT_MENU(menu_name)                                                                                           \
  extern void menu_name##_init();                                                                                      \
  menu_name##_init();

#define OPEN_MENU(ui_renderer, menu_name)                                                                              \
  extern void menu_name##_open(UiRenderer *renderer, const ClientGame *game);                                                \
  menu_name##_open(ui_renderer, game);

void game_init_menu(ClientGame *game) {
  INIT_MENU(save_menu);
  INIT_MENU(dialog_menu);
  // INIT_MENU(start_menu);
  // INIT_MENU(debug_menu);
}

bool game_cur_menu_hides_game(ClientGame *game);

void game_render_menu(ClientGame *game);

static void game_open_menu(ClientGame *game, MenuId menu_id) {
  switch (menu_id) {
  case MENU_NEW_SAVE: {
    OPEN_MENU(&game->ui_renderer, new_save_menu);
    break;
  }
  case MENU_HOST_SERVER: {
    OPEN_MENU(&game->ui_renderer, host_menu);
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
    game_render_menu(&CLIENT_GAME);
    ui_renderer->ui_height = ui_renderer->cur_y;

    ui_renderer->simulate = false;
    ui_renderer->cur_x = 0;
    ui_renderer->cur_y = 0;
  }
}

void game_set_menu(ClientGame *game, MenuId menu_id) {
  game->cur_menu = menu_id;
  game_calc_ui_height(&game->ui_renderer);
  game_open_menu(game, menu_id);
}

bool game_cur_menu_hides_game(ClientGame *game) {
  return game->cur_menu == MENU_START || game->cur_menu == MENU_NEW_SAVE || game->cur_menu == MENU_LOAD_SAVE ||
      game->cur_menu == MENU_MULTIPLAYER || game->cur_menu == MENU_HOST_SERVER;
}
