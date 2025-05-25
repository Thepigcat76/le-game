#pragma once

#include "being.h"
#include "data.h"
#include "debug.h"
#include "item.h"
#include "menu.h"
#include "particle.h"
#include "player.h"
#include "shared.h"
#include "ui.h"
#include "world.h"

typedef struct _game {
  Player player;
  World world;
  MenuId cur_menu;
  bool paused;
  // LOADING
  int detected_saves;
  // RENDERING
  ParticleManager particle_manager;
  UiRenderer ui_renderer;
  // DEBUGGING
  DebugOptions debug_options;
} Game;

extern Game GAME;

void game_create_world(Game *game, float seed);

void game_detect_saves(Game *game);

void game_reload();

void game_tick(Game *game);

// GAME RENDER

void game_render(Game *game);

void game_render_overlay(Game *game);

// GAME LOAD/SAVE

void game_load(Game *game);

void game_unload(Game *game);

// MENUS

bool game_cur_menu_hides_game(Game *game);

void game_init_menu(Game *game);

void game_render_menu(Game *game);

void game_set_menu(Game *game, MenuId menu_id);

// PARTICLES

ParticleInstance *game_emit_particle_ex(Game *game, ParticleInstance particle_instance);

ParticleInstance *game_emit_particle(Game *game, int x, int y, ParticleId particle_id,
                                     ParticleInstanceEx particle_extra);

void game_render_particle(Game *game, ParticleInstance particle, bool behind_player);

void game_render_particles(Game *game, bool behind_player);
