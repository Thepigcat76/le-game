#pragma once

#include "data.h"
#include "item.h"
#include "shared.h"
#include "tile.h"
#include <raylib.h>

typedef struct {
  Camera2D cam;
  Vec2f cur_cam_pos;
  Vec2f prev_cam_pos;
  Vec2f cur_box_pos;
  Vec2f prev_box_pos;
  Texture2D animated_textures[DIRECTIONS_AMOUNT];
  Texture2D textures[DIRECTIONS_AMOUNT];
  Direction direction;
  int animation_frame;
  int frame_timer;
  bool walking;
  bool in_water;
  TileInstance last_broken_tile;
  // 0 - 64 ( / 16 for anim frames)
  int break_progress;
  // Tile player is currently breaking
  TileInstance break_tile;
  TilePos break_tile_pos;

  // PLAYER DATA
  Rectf box;
  int essence;
  TilePos tile_pos;
  ChunkPos chunk_pos;
  ItemInstance held_item;
} Player;

Player player_new();

Rectf player_collision_box(const Player *player);

void player_tick(Player *player);

void player_render(Player *player, float alpha);

void player_set_pos(Player *player, float x, float y);

void player_handle_zoom(Player *player, bool zoom_in, bool zoom_out);

void player_handle_movement(Player *player, bool w, bool a, bool s, bool d);

void player_set_pos_ex(Player *player, float x, float y, bool update_chunk, bool walking_particles,
                       bool check_for_water);

void player_load(Player *player, DataMap *map);

void player_save(Player *player, DataMap *map);
