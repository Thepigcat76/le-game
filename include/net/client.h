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
  // a save on the server
  Save *cur_save;
  // Pointer to the common game struct
  struct _game *game;
  // World, Player - points to the
  // world and player usually stored
  // in CLIENT_GAME.cur_save
  World *world;
  Player *player;
} ClientGame;

extern ClientGame CLIENT_GAME;

void client_init(void);

void client_deinit(ClientGame *game);

void client_init_raylib(void);

void client_deinit_raylib(void);

// TICKING

void client_tick(ClientGame *game);

// RELOADING

void client_reload(ClientGame *game);

// MENUS

void client_init_menu(ClientGame *game);

bool client_cur_menu_hides_game(ClientGame *game);

void client_render_menu(ClientGame *game);

void client_set_menu(ClientGame *game, MenuId menu_id);

// GAME RENDER

void client_render(ClientGame *game, float alpha);

void client_render_overlay(ClientGame *game);

// PARTICLES

void client_render_particle(ClientGame *game, ParticleInstance particle, bool behind_player);

void client_render_particles(ClientGame *game, bool behind_player);

ParticleInstance *client_emit_particle_ex(ClientGame *game, ParticleInstance particle_instance);

ParticleInstance *client_emit_particle(ClientGame *game, int x, int y, ParticleId particle_id,
                                     ParticleInstanceEx particle_extra);