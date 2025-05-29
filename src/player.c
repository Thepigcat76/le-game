#include "../include/player.h"
#include "../include/camera.h"
#include "../include/config.h"
#include "../include/game.h"
#include "../include/shared.h"
#include "math.h"
#include <raylib.h>

Texture2D particle_texture0;

Player player_new() {
  particle_texture0 = load_texture("res/assets/walk_particles.png");
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
                  .box = {.x = 0, .y = 20, .width = 16, .height = 12},
                  .chunk_pos = vec2i(0, 0),
                  .tile_pos = vec2i(0, 0),
                  .break_progress = -1,
                  .break_tile_pos = vec2i(0, 0)};
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
  DrawTexturePro(player_texture,
                 (Rectangle){0, player->walking ? 32 * player->animation_frame : 32, 16, player->in_water ? 24 : 32},
                 (Rectangle){.x = player->box.x + 8 * scale,
                             .y = player->box.y + 16 * scale,
                             .width = 16 * scale,
                             .height = (player->in_water ? 24 : 32) * scale},
                 (Vector2){.x = 8 * scale, .y = 16 * scale}, 0, WHITE);

  if (player->walking) {
    update_animation(player, GetFrameTime());
  }
}

void player_set_pos_ex(Player *player, float x, float y, bool update_chunk, bool walking_particles,
                       bool check_for_water) {
  player->in_water = world_ground_tile_at(player->world, player->tile_pos)->type.id == TILE_WATER;
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

#define CHECK_COLLISIONS_ENABLED(direction)                                                                            \
  if (!GAME.debug_options.collisions_enabled)                                                                          \
    player->collisions[direction] = false;                                                                             \
  else

void player_handle_movement(Player *player, bool w, bool a, bool s, bool d) {
  Camera2D *cam = &player->cam;

  float distance = 2 * CONFIG.player_speed;

  bool walking = false;

  TilePos player_tile_pos = player->tile_pos;
  Rectangle player_hitbox = player_collision_box(player);

  if (w) {
    TilePos tile_pos = vec2i_add(player_tile_pos, vec2i(0, 0));
    TileInstance *tile = world_tile_at(player->world, tile_pos, TILE_LAYER_TOP);
#ifdef SURTUR_DEBUG
    CHECK_COLLISIONS_ENABLED(DIRECTION_UP)
#endif
    {
      player->collisions[DIRECTION_UP] =
          (tile->type.id != TILE_EMPTY &&
           CheckCollisionRecs(rectf_from_dimf(tile_pos.x * TILE_SIZE, tile_pos.y * TILE_SIZE + 8, tile->box),
                              rec_offset_direction(player_hitbox, DIRECTION_UP, distance)));
    }
    player->direction = DIRECTION_UP;

    if (!player->collisions[DIRECTION_UP]) {
      player_set_pos(player, player_pos(player).x, player_pos(player).y - distance);
      walking = true;
    }
  }
  if (s) {
    TilePos tile_pos = vec2i_add(player_tile_pos, vec2i(0, 0));
    TileInstance *tile = world_tile_at(player->world, tile_pos, TILE_LAYER_TOP);
#ifdef SURTUR_DEBUG
    CHECK_COLLISIONS_ENABLED(DIRECTION_DOWN)
#endif
    {
      TraceLog(LOG_DEBUG, "tile: %s, Rec w: %d, Rec h: %d", tile_type_to_string(&tile->type), tile->box.width, tile->box.height);
      player->collisions[DIRECTION_DOWN] = tile->type.id != TILE_EMPTY &&
          CheckCollisionRecs(rectf_from_dimf(tile_pos.x * TILE_SIZE, tile_pos.y * TILE_SIZE + 8, tile->box),
                             rec_offset_direction(player_hitbox, DIRECTION_DOWN, distance));
    }
    player->direction = DIRECTION_DOWN;

    if (!player->collisions[DIRECTION_DOWN]) {
      player_set_pos(player, player_pos(player).x, player_pos(player).y + distance);
      walking = true;
    }
  }
  if (a) {
    TilePos tile_pos = vec2i_add(player_tile_pos, vec2i(0, 0));
    TileInstance *tile = world_tile_at(player->world, tile_pos, TILE_LAYER_TOP);
// TileInstance *tile_above = world_tile_at(player->world, vec2i_add(tile_pos, vec2i(-1, -1)), TILE_LAYER_TOP);
#ifdef SURTUR_DEBUG
    CHECK_COLLISIONS_ENABLED(DIRECTION_LEFT)
#endif
    {
      Rectf tile_box = rec_offset_direction(rectf_from_dimf(tile_pos.x * TILE_SIZE, tile_pos.y * TILE_SIZE + 8, tile->box),
                                            DIRECTION_LEFT, distance);
      rec_draw_outline(tile_box, ORANGE);
      player->collisions[DIRECTION_LEFT] = (tile->type.id != TILE_EMPTY && CheckCollisionRecs(player_hitbox, tile_box));
    }
    //||
    //  (tile_above->type.id != TILE_EMPTY && CheckCollisionRecs(player->box, tile_above->box));
    player->direction = DIRECTION_LEFT;

    if (!player->collisions[DIRECTION_LEFT]) {
      player_set_pos(player, player_pos(player).x - distance, player_pos(player).y);
      walking = true;
    }
  }
  if (d) {
    TilePos tile_pos = vec2i_add(player_tile_pos, vec2i(0, 0));
    TileInstance *tile = world_tile_at(player->world, tile_pos, TILE_LAYER_TOP);

#ifdef SURTUR_DEBUG
    CHECK_COLLISIONS_ENABLED(DIRECTION_RIGHT)
#endif
    {
      player->collisions[DIRECTION_RIGHT] =
          (tile->type.id != TILE_EMPTY &&
           CheckCollisionRecs(
               player_hitbox,
               rec_offset_direction(rectf_from_dimf(tile_pos.x * TILE_SIZE, tile_pos.y * TILE_SIZE + 8, tile->box),
                                    DIRECTION_RIGHT, distance)));
    }
    player->direction = DIRECTION_RIGHT;

    if (!player->collisions[DIRECTION_RIGHT]) {
      player_set_pos(player, player_pos(player).x + distance, player_pos(player).y);
      walking = true;
    }
  }

  player->walking = walking;
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
