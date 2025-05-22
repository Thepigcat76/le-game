#include "../include/player.h"
#include "../include/camera.h"
#include "../include/config.h"
#include "../include/shared.h"
#include "math.h"
#include <raylib.h>

Player player_new() {
  return (Player){.world = NULL,
                  .cam = camera_new(SCREEN_WIDTH, SCREEN_HEIGHT),
                  .animated_textures = {load_texture("res/assets/player_front_walk.png"),
                                        load_texture("res/assets/player_back_walk.png"),
                                        load_texture("res/assets/player_left_walk.png"),
                                        load_texture("res/assets/player_right_walk.png")},
                  .textures = {load_texture("res/assets/player_front.png"), load_texture("res/assets/player_back.png"),
                               load_texture("res/assets/player_left.png"), load_texture("res/assets/player_right.png")},
                  .direction = DIRECTION_DOWN,
                  .last_broken_tile = TILE_INSTANCE_EMPTY,
                  .essence = 0,
                  .animation_frame = 0,
                  .frame_timer = 0,
                  .held_item = {.type = ITEMS[ITEM_GRASS]},
                  .box = {.x = 0, .y = 0, .width = 16, .height = 32},
                  .chunk_pos = vec2i(0, 0),
                  .tile_pos = vec2i(0, 0)};
}

void player_set_world(Player *player, World *world) { player->world = world; }

static Texture2D player_get_texture(Player *player) {
  Texture2D *textures;
  if (player->walking) {
    textures = player->animated_textures;
  } else {
    textures = player->textures;
  }
  switch (player->direction) {
  case DIRECTION_DOWN:
    return textures[0];
  case DIRECTION_UP:
    return textures[1];
  case DIRECTION_LEFT:
    return textures[2];
  case DIRECTION_RIGHT:
    return textures[3];
  }
}

Vector2 player_pos(const Player *player) { return (Vector2){.x = player->box.x, .y = player->box.y}; }

static void update_animation(Player *player, float deltaTime) {
  player->frame_timer += deltaTime * 1000.0f; // to ms
  float delay = 125;
  if (player->frame_timer >= delay) {
    player->frame_timer = 0.0f;
    player->animation_frame = (player->animation_frame + 1) % 5;
  }
}

void player_render(Player *player) {
  double scale = 1;
  Texture2D player_texture = player_get_texture(player);
  DrawTexturePro(
      player_texture, (Rectangle){0, player->walking ? 32 * player->animation_frame : 32, 16, 32},
      (Rectangle){
          .x = player->box.x + 8 * scale, .y = player->box.y + 16 * scale, .width = 16 * scale, .height = 32 * scale},
      (Vector2){.x = 8 * scale, .y = 16 * scale}, 0, WHITE);

  if (player->walking) {
    update_animation(player, GetFrameTime());
  }
}

void player_set_pos_ex(Player *player, float x, float y, bool update_chunk) {
  ChunkPos old_chunk_pos = player->chunk_pos;
  player->box.x = x;
  player->box.y = y;
  player->cam.target.x = x;
  player->cam.target.y = y;
  player->tile_pos.x = floor_div(x, TILE_SIZE);
  player->tile_pos.y = floor_div(y, TILE_SIZE) + 1;
  player->chunk_pos.x = floor_div(x, CHUNK_SIZE * TILE_SIZE);
  player->chunk_pos.y = floor_div(y, CHUNK_SIZE * TILE_SIZE);

  if (old_chunk_pos.x != player->chunk_pos.x || old_chunk_pos.y != player->chunk_pos.y) {
    TraceLog(LOG_INFO, "Entering New chunk");
  }

  if (update_chunk && !world_has_chunk_at(player->world, player->chunk_pos)) {
    world_gen_chunk_at(player->world, player->chunk_pos);

    world_prepare_chunk_rendering(player->world,
                                  &player->world->chunks[world_chunk_index_by_pos(player->world, player->chunk_pos)]);
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
              player->world, vec2i(player->chunk_pos.x + offset.x, player->chunk_pos.y + offset.y))]);
    }
  }
}

void player_set_pos(Player *player, float x, float y) { player_set_pos_ex(player, x, y, true); }

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

  bool walking = false;

  if (w) {
    player_set_pos(player, player_pos(player).x, player_pos(player).y - distance);
    player->direction = DIRECTION_UP;
    walking = true;
  }
  if (s) {
    player_set_pos(player, player_pos(player).x, player_pos(player).y + distance);
    player->direction = DIRECTION_DOWN;
    walking = true;
  }
  if (a) {
    player_set_pos(player, player_pos(player).x - distance, player_pos(player).y);
    player->direction = DIRECTION_LEFT;
    walking = true;
  }
  if (d) {
    player_set_pos(player, player_pos(player).x + distance, player_pos(player).y);
    player->direction = DIRECTION_RIGHT;
    walking = true;
  }

  player->walking = walking;
}

void player_load(Player *player, DataMap *map) {
  player->essence = data_map_get_or_default(map, "essence", data_int(0)).var.data_int;
  player->direction = data_map_get_or_default(map, "direction", data_int(0)).var.data_int;
  int x = data_map_get_or_default(map, "pos_x", data_int(0)).var.data_int;
  int y = data_map_get_or_default(map, "pos_y", data_int(0)).var.data_int;
  player->box.x = x;
  player->box.y = y;
  player->cam.target.x = x;
  player->cam.target.y = y;
}

void player_save(Player *player, DataMap *map) {
  data_map_insert(map, "essence", data_int(player->essence));
  data_map_insert(map, "direction", data_int(player->direction));
  data_map_insert(map, "pos_x", data_int(player->box.x));
  data_map_insert(map, "pos_y", data_int(player->box.y));
}
