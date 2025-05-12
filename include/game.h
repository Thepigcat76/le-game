#pragma once

#include "data.h"
#include "player.h"
#include "world.h"
#include "item.h"

typedef struct {
  Player player;
  World world;
} Game;

void game_reload();

void game_tick(Game *game);

void game_free(const Game *game);

void load_game(Game *game, ByteBuf *bytebuf);

void save_game(Game *game, ByteBuf *bytebuf);
