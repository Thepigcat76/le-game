#pragma once

#include "../keys.h"
#include "../menu.h"
#include "../particle.h"
#include "../player.h"
#include "../shaders.h"
#include "../sounds.h"
#include "../textures.h"
#include "../ui.h"
#include "../window.h"
#include "../world.h"
#include "common.h"

typedef struct {
  PlayerDescriptor *server_players;
} ServerData;

typedef struct {
  MenuId cur_menu;
  bool paused;
  // Managers
  TextureManager texture_manager;
  ShaderManager shader_manager;
  SoundManager sound_manager;
  ParticleManager particle_manager;
  UiRenderer ui_renderer;
  Window window;
  RenderTexture2D world_texture;
  bool slot_selected;
  PressedKeys pressed_keys;
  bool singleplayer;
  // World, Player - points either to the
  // world and player stored in the save
  // or world and player in a special server
  // save
  World *world;
  Player *player;
} ClientGame;

extern ClientGame CLIENT_GAME;

void client_init(void);

void game_client_reload(ClientGame *game);

// MENUS

void game_init_menu(ClientGame *game);

bool game_cur_menu_hides_game(ClientGame *game);

void game_render_menu(ClientGame *game);

void game_set_menu(ClientGame *game, MenuId menu_id);