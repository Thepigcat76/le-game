#include "../include/player.h"
#include "../include/camera.h"
#include "../include/config.h"
#include "../include/shared.h"
#include "math.h"
#include <raylib.h>

Player player_new() {
  return (Player){.world = NULL,
                  .cam = camera_new(SCREEN_WIDTH, SCREEN_HEIGHT),
                  .textures = {load_texture("res/assets/player.png"),
                               load_texture("res/assets/player_back.png"),
                               load_texture("res/assets/player_left.png"),
                               load_texture("res/assets/player_right.png")},
                  .direction = DIRECTION_DOWN,
                  .essence = 0,
                  .held_item = {.type = ITEMS[ITEM_STICK]},
                  .box = {.x = 0, .y = 0, .width = 16, .height = 32}};
}

void player_set_world(Player *player, World *world) { player->world = world; }

Texture2D player_get_texture(Player *player) {
  switch (player->direction) {
  case DIRECTION_DOWN:
    return player->textures[0];
  case DIRECTION_UP:
    return player->textures[1];
  case DIRECTION_LEFT:
    return player->textures[2];
  case DIRECTION_RIGHT:
    return player->textures[3];
  }
}

Vector2 player_pos(const Player *player) {
  return (Vector2){.x = player->box.x, .y = player->box.y};
}

void player_set_pos(Player *player, float x, float y) {
  ChunkPos old_chunk_pos = player->chunk_pos;
  player->box.x = x;
  player->box.y = y;
  player->cam.target.x = x;
  player->cam.target.y = y;
  player->chunk_pos.x = x / (CHUNK_SIZE * TILE_SIZE);
  player->chunk_pos.y = y / (CHUNK_SIZE * TILE_SIZE);

  if (!world_has_chunk_at(player->world, player->chunk_pos)) {
    world_gen_chunk_at(player->world, player->chunk_pos);

    world_prepare_chunk_rendering(
        player->world, &player->world->chunks[world_chunk_index_by_pos(
                           player->world, player->chunk_pos)]);
    Vec2i offsets[4] = {
        vec2i(-1, 0),
        vec2i(+1, 0),
        vec2i(0, -1),
        vec2i(0, +1),
    };
    for (int i = 0; i < 4; i++) {
      Vec2i offset = offsets[i];
      world_prepare_chunk_rendering(
          player->world,
          &player->world->chunks[world_chunk_index_by_pos(
              player->world,
              vec2i(player->chunk_pos.x + offset.x, player->chunk_pos.y + offset.y))]);
    }
  }
}

void player_handle_zoom(Player *player, bool zoom_in, bool zoom_out) {
  Camera2D *cam = &player->cam;

  if (zoom_in) {
    cam->zoom = fmin(cam->zoom + GetFrameTime() * 2.0, 4);
  }

  if (zoom_out) {
    cam->zoom = fmax(cam->zoom - GetFrameTime() * 2.0, 1);
  }
}

void player_handle_movement(Player *player, bool w, bool a, bool s, bool d) {
  Camera2D *cam = &player->cam;

  float distance = 2 * CONFIG.player_speed;

  if (w) {
    player_set_pos(player, player_pos(player).x,
                   player_pos(player).y - distance);
    player->direction = DIRECTION_UP;
  }
  if (a) {
    player_set_pos(player, player_pos(player).x - distance,
                   player_pos(player).y);
    player->direction = DIRECTION_LEFT;
  }
  if (s) {
    player_set_pos(player, player_pos(player).x,
                   player_pos(player).y + distance);
    player->direction = DIRECTION_DOWN;
  }
  if (d) {
    player_set_pos(player, player_pos(player).x + distance,
                   player_pos(player).y);
    player->direction = DIRECTION_RIGHT;
  }
}

void load_player(Player *player, DataMap *map) {
  player->essence =
      data_map_get_or_default(map, "essence", data_int(0)).var.data_int;
  player->direction =
      data_map_get_or_default(map, "direction", data_int(0)).var.data_int;
  int x = data_map_get_or_default(map, "pos_x", data_int(0)).var.data_int;
  int y = data_map_get_or_default(map, "pos_y", data_int(0)).var.data_int;
  player->box.x = x;
  player->box.y = y;
  player->cam.target.x = x;
  player->cam.target.y = y;
}

void save_player(Player *player, DataMap *map) {
  data_map_insert(map, "essence", data_int(player->essence));
  data_map_insert(map, "direction", data_int(player->direction));
  data_map_insert(map, "pos_x", data_int(player->box.x));
  data_map_insert(map, "pos_y", data_int(player->box.y));
}
