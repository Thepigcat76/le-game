#include "../include/player.h"
#include "../include/camera.h"
#include "../include/config.h"
#include "../include/game.h"
#include "../include/shared.h"
#include "math.h"
#include <raylib.h>

#define WORLD_PTR (&GAME.world)

Texture2D particle_texture0;

static Rectf TILE_BOX = {};

Player player_new() {
  particle_texture0 = LoadTexture("res/assets/walk_particles.png");
  return (Player){.cam = camera_new(SCREEN_WIDTH, SCREEN_HEIGHT),
                  .animated_textures = {LoadTexture("res/assets/player_front_walk.png"),
                                        LoadTexture("res/assets/player_back_walk.png"),
                                        LoadTexture("res/assets/player_left_walk.png"),
                                        LoadTexture("res/assets/player_right_walk.png")},
                  .textures = {LoadTexture("res/assets/player_front.png"), LoadTexture("res/assets/player_back.png"),
                               LoadTexture("res/assets/player_left.png"), LoadTexture("res/assets/player_right.png")},
                  .direction = DIRECTION_DOWN,
                  .last_broken_tile = TILE_INSTANCE_EMPTY,
                  .essence = 0,
                  .animation_frame = 0,
                  .frame_timer = 0,
                  .held_item = {.type = ITEMS[ITEM_GRASS]},
                  .box = {.x = 0, .y = 20, .width = 16, .height = 12},
                  .chunk_pos = vec2i(0, 0),
                  .tile_pos = vec2i(0, 0),
                  .break_progress = -1,
                  .break_tile_pos = vec2i(0, 0)};
}

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
  DrawTexturePro(player_texture,
                 (Rectangle){0, player->walking ? 32 * player->animation_frame : 32, 16, player->in_water ? 24 : 32},
                 (Rectangle){.x = player->box.x + 8 * scale,
                             .y = player->box.y + 16 * scale,
                             .width = 16 * scale,
                             .height = (player->in_water ? 24 : 32) * scale},
                 (Vector2){.x = 8 * scale, .y = 16 * scale}, 0, WHITE);

  rec_draw_outline(TILE_BOX, ORANGE);

  if (player->walking) {
    update_animation(player, GetFrameTime());
  }
}

void player_set_pos_ex(Player *player, float x, float y, bool update_chunk, bool walking_particles,
                       bool check_for_water) {
  player->in_water = world_ground_tile_at(WORLD_PTR, player->tile_pos)->type.id == TILE_WATER;
  if (check_for_water && player->in_water) {
    x -= (x - player->box.x) / 2;
    y -= (y - player->box.y) / 2;
  }

  ChunkPos old_chunk_pos = player->chunk_pos;
  player->box.x = x;
  player->box.y = y;
  player->cam.target.x = x;
  player->cam.target.y = y;
  player->tile_pos.x = floor_div(x + 8, TILE_SIZE);
  player->tile_pos.y = floor_div(y + 8, TILE_SIZE) + 1;
  player->chunk_pos.x = floor_div(x, CHUNK_SIZE * TILE_SIZE);
  player->chunk_pos.y = floor_div(y, CHUNK_SIZE * TILE_SIZE);

  if (update_chunk && !world_has_chunk_at(WORLD_PTR, player->chunk_pos)) {
    world_gen_chunk_at(WORLD_PTR, player->chunk_pos);

    world_prepare_chunk_rendering(WORLD_PTR,
                                  &WORLD_PTR->chunks[world_chunk_index_by_pos(WORLD_PTR, player->chunk_pos)]);
    Vec2i offsets[4] = {
        vec2i(-1, 0),
        vec2i(+1, 0),
        vec2i(0, -1),
        vec2i(0, +1),
    };
    for (int i = 0; i < 4; i++) {
      Vec2i offset = offsets[i];
      world_prepare_chunk_rendering(
          WORLD_PTR,
          &WORLD_PTR->chunks[world_chunk_index_by_pos(
              WORLD_PTR, vec2i(player->chunk_pos.x + offset.x, player->chunk_pos.y + offset.y))]);
    }
  }

  if (walking_particles && GetRandomValue(0, 4) == 0) {
    TileInstance *tile = world_ground_tile_at(&GAME.world, player->tile_pos);
    ParticleInstance *particle = game_emit_particle(
        &GAME, x + GetRandomValue(-5, 7), y + GetRandomValue(-5, 7) + 27, PARTICLE_WALKING,
        (ParticleInstanceEx){.type = PARTICLE_INSTANCE_WALKING,
                             .var = {.tile_break = {.texture = particle_texture0, .tint = tile->type.tile_color}}});
    particle->lifetime /= 1.5;
    particle->velocity = vec2f(0, 0);
  }
}

void player_set_pos(Player *player, float x, float y) { player_set_pos_ex(player, x, y, true, true, true); }

void player_handle_zoom(Player *player, bool zoom_in, bool zoom_out) {
  Camera2D *cam = &player->cam;

  if (zoom_in) {
    cam->zoom = fmin(cam->zoom + GetFrameTime() * 2.0, 4);
  }

  if (zoom_out) {
    cam->zoom = fmax(cam->zoom - GetFrameTime() * 2.0, 1);
  }
}

Rectangle rec_offset(Rectangle rectangle, int32_t x_offset, int32_t y_offset, int32_t width_offset,
                     int32_t height_offset) {
  return (Rectangle){
      .x = rectangle.x + x_offset,
      .y = rectangle.y + y_offset,
      .width = rectangle.width + width_offset,
      .height = rectangle.height + height_offset,
  };
}

Rectangle rec_offset_direction(Rectangle rec, Direction direction, int32_t distance) {
  switch (direction) {
  case DIRECTION_RIGHT:
    return rec_offset(rec, distance, 0, 0, 0);
  case DIRECTION_LEFT:
    return rec_offset(rec, -distance, 0, 0, 0);
  case DIRECTION_UP:
    return rec_offset(rec, 0, -distance, 0, 0);
  case DIRECTION_DOWN:
    return rec_offset(rec, 0, distance, 0, 0);
  }
}

void player_handle_movement(Player *player, bool w, bool a, bool s, bool d) {
  Camera2D *cam = &player->cam;

  float distance = 2 * CONFIG.player_speed;

  bool walking = false;

  TilePos player_tile_pos = player->tile_pos;
  Rectangle player_hitbox = player_collision_box(player);

  Vec2f player_move = vec2f(0, 0);
  Vec2f player_pos_copy = player_pos(player);

  if (w) {
    player->direction = DIRECTION_UP;

    player_move.y = -distance;
    walking = true;
  }
  if (s) {
    player->direction = DIRECTION_DOWN;

    player_move.y = distance;
    walking = true;
  }
  if (a) {
    player->direction = DIRECTION_LEFT;

    player_move.x = -distance;
    walking = true;
  }
  if (d) {
    player->direction = DIRECTION_RIGHT;

    player_move.x = distance;
    walking = true;
  }

  if (walking) {
    if (a || d) {
      player_pos_copy.x = player_pos(player).x + player_move.x;
    }
    if (w || s)
      player_pos_copy.y = player_pos(player).y + player_move.y;
  }

  if (walking) {
    for (int y = -1; y < 1; y++) {
      for (int x = -1; x < 1; x++) {
        TilePos cur_pos = vec2i(player_tile_pos.x + x, player_tile_pos.y + y);
        TileInstance *tile = world_tile_at(WORLD_PTR, cur_pos, TILE_LAYER_TOP);
        if (tile->type.id != TILE_EMPTY && !(x == 0 && y == 0)) {
          Rectf tile_box= tile_collision_box_at(tile, cur_pos.x * TILE_SIZE, cur_pos.y * TILE_SIZE);
          if (CheckCollisionRecs(player_hitbox, tile_box)) {
            TraceLog(LOG_DEBUG, "Colliding");
            if (player_move.x > 0) {
              player_pos_copy.x = tile_box.x - player_hitbox.width;
            }
            if (player_move.x < 0) {
              player_pos_copy.x = tile_box.x + tile_box.width;
            }
            if (player_move.y > 0) {
              player_pos_copy.y = tile_box.y - player_hitbox.height;
            }
            if (player_move.y < 0) {
              player_pos_copy.y = tile_box.y + tile_box.height;
            }
            break;
          }
        }
      }
    }
  }

  player->walking = walking;

  if (walking) {
    player_set_pos(player, player_pos_copy.x, player_pos_copy.y);
  }
}

Rectf player_collision_box(const Player *player) {
  return rectf(player->box.x, player->box.y + 20, player->box.width, player->box.height);
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
