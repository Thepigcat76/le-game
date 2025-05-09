#include "../include/player.h"
#include "../include/camera.h"
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
                  .box = {.x = 0, .y = 0, .width = 16, .height = 32}};
}

void player_set_world(Player *player, World *world) {
  player->world = world;
}

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

void player_set_pos(Player *player, int x, int y) {
  player->box.x = x;
  player->box.y = y;
  player->cam.target.x = x;
  player->cam.target.y = y;
  player->chunk_pos.x = x / (CHUNK_SIZE * TILE_SIZE);
  player->chunk_pos.y = y / (CHUNK_SIZE * TILE_SIZE);

  if (!world_has_chunk_at(player->world, player->chunk_pos)) {
    world_gen_chunk_at(player->world, player->chunk_pos);
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

  if (w) {
    player_set_pos(player, player_pos(player).x, player_pos(player).y - 2);
    player->direction = DIRECTION_UP;
  }
  if (a) {
    player_set_pos(player, player_pos(player).x - 2, player_pos(player).y);
    player->direction = DIRECTION_LEFT;
  }
  if (s) {
    player_set_pos(player, player_pos(player).x, player_pos(player).y + 2);
    player->direction = DIRECTION_DOWN;
  }
  if (d) {
    player_set_pos(player, player_pos(player).x + 2, player_pos(player).y);
    player->direction = DIRECTION_RIGHT;
  }
}

void load_player(Player *player, DataMap *map) {
  player->essence =
      data_map_get_or_default(map, "essence", data_int(0)).var.data_int;
  player->direction =
      data_map_get_or_default(map, "direction", data_int(0)).var.data_int;
}

void save_player(Player *player, DataMap *map) {
  data_map_insert(map, "essence", data_int(player->essence));
  data_map_insert(map, "direction", data_int(player->direction));
}
