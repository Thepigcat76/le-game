#pragma once

#include "data.h"
#include "item.h"
#include "menu.h"
#include "player.h"
#include "world.h"

typedef struct _game {
  Player player;
  World world;
  MenuId cur_menu;
  bool paused;
} Game;

void game_reload();

void game_tick(Game *game);

void game_unload(Game *game);

void game_free(const Game *game);

void load_game(Game *game, ByteBuf *bytebuf);

void save_game(Game *game, ByteBuf *bytebuf);
