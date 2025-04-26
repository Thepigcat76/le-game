#pragma once

#include "shared.h"
#include "data.h"
#include <raylib.h>

typedef struct {
  Camera2D cam;
  Texture2D textures[4];
  Direction direction;

  // PLAYER DATA
  int essence;
} Player;

Player player_new();

Texture2D player_get_texture(Player *player);

void player_handle_zoom(Player *player, bool zoom_in, bool zoom_out);

void player_handle_movement(Player *player, bool w, bool a, bool s, bool d);

void load_player(Player *player, DataMap *map);

void save_player(Player *player, DataMap *map);
