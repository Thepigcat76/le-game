#pragma once

#include "../keys.h"
#include "../menu.h"
#include "../particle.h"
#include "../player.h"
#include "../save.h"
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
  // Saves
  // These are saves that are actually stored on disk
  SaveDescriptor *local_saves;
  // A save - this can either be a local save or
  Save cur_save;
  // World, Player - points to the
  // world and player usually stored
  // in ClientGame.cur_save
  World *world;
  Player *player;
} ClientGame;

extern ClientGame CLIENT_GAME;

void client_init(void);

void game_init_raylib(void);

void game_deinit_raylib(void);

// RELOADING

void game_client_reload(ClientGame *game);

// MENUS

void game_init_menu(ClientGame *game);

bool game_cur_menu_hides_game(ClientGame *game);

void game_render_menu(ClientGame *game);

void game_set_menu(ClientGame *game, MenuId menu_id);

// GAME RENDER

void game_render(ClientGame *game, float alpha);

void game_render_overlay(ClientGame *game);