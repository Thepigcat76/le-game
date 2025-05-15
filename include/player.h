#pragma once

#include "item.h"
#include "shared.h"
#include "data.h"
#include "world.h"
#include <raylib.h>

typedef struct {
  World *world;
  Camera2D cam;
  Texture2D textures[4];
  Direction direction;

  // PLAYER DATA
  Rectangle box;
  int essence;
  Vec2i chunk_pos;
  ItemInstance held_item;
} Player;

Player player_new();

void player_set_world(Player *player, World *world);

Texture2D player_get_texture(Player *player);

void player_set_pos(Player *player, float x, float y);

void player_handle_zoom(Player *player, bool zoom_in, bool zoom_out);

void player_handle_movement(Player *player, bool w, bool a, bool s, bool d);

void load_player(Player *player, DataMap *map);

void save_player(Player *player, DataMap *map);
