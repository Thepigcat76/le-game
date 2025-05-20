#pragma once

#include "being.h"
#include "data.h"
#include "item.h"
#include "menu.h"
#include "player.h"
#include "shared.h"
#include "world.h"

typedef struct _game {
  Player player;
  World world;
  MenuId cur_menu;
  bool paused;
} Game;

void game_reload();

void game_render(Game *game);

void game_tick(Game *game);

void game_load(Game *game);

void game_unload(Game *game);

void game_free(const Game *game);
