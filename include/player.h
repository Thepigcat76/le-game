#pragma once

#include "item.h"
#include "shared.h"
#include "data.h"
#include "tile.h"
#include "world.h"
#include <raylib.h>

typedef struct {
  World *world;
  Camera2D cam;
  Texture2D animated_textures[DIRECTIONS_AMOUNT];
  Texture2D textures[DIRECTIONS_AMOUNT];
  Direction direction;
  int animation_frame;
  int frame_timer;
  bool walking;
  bool in_water;
  TileInstance last_broken_tile;

  // PLAYER DATA
  Rectangle box;
  int essence;
  TilePos tile_pos;
  ChunkPos chunk_pos;
  ItemInstance held_item;
} Player;

Player player_new();

void player_set_world(Player *player, World *world);

void player_render(Player *player);

void player_set_pos(Player *player, float x, float y);

void player_handle_zoom(Player *player, bool zoom_in, bool zoom_out);

void player_handle_movement(Player *player, bool w, bool a, bool s, bool d);

void player_set_pos_ex(Player *player, float x, float y, bool update_chunk, bool walking_particles, bool check_for_water);

void player_load(Player *player, DataMap *map);

void player_save(Player *player, DataMap *map);
