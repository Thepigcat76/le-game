#pragma once

#include "being.h"
#include "data.h"
#include "item.h"
#include "menu.h"
#include "player.h"
#include "shared.h"
#include "world.h"
#include "particle.h"

extern  ParticleManager PARTICLE_MANAGER;

typedef struct _game {
  Player player;
  World world;
  MenuId cur_menu;
  bool paused;
} Game;

void game_reload();

// TODO: We need to move ui renderer to a struct
void game_set_menu(Game *game, MenuId menu_id);

void game_render(Game *game);

void game_tick(Game *game);

void game_load(Game *game);

void game_unload(Game *game);

// PARTICLES

void game_emit_particle(int x, int y, ParticleId particle_id, ParticleInstanceEx particle_extra);

void game_render_particles();
