#pragma once

#include "being.h"
#include "data.h"
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
  ParticleManager particle_manager;
  UiRenderer ui_renderer;
} Game;

extern Game GAME;

void game_reload();

void game_render(Game *game);

void game_tick(Game *game);

void game_load(Game *game);

void game_unload(Game *game);

// MENUS

void game_render_menu(Game *game);

void game_set_menu(Game *game, MenuId menu_id);

// PARTICLES

void game_emit_particle(Game *game, int x, int y, ParticleId particle_id, ParticleInstanceEx particle_extra);

void game_render_particles(Game *game);
