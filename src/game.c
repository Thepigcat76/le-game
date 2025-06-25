#include "../include/game.h"
#include "../include/alloc.h"
#include "../include/camera.h"
#include "../include/config.h"
#include "../include/item/item_container.h"
#include "raylib.h"
#include <dirent.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#define RELOAD(game_ptr, src_file_prefix)                                                                              \
  extern void src_file_prefix##_on_reload(Game *game);                                                                 \
  src_file_prefix##_on_reload(game_ptr);

static bool game_slot_selected();

void game_reload(Game *game) {
  RELOAD(game, tile);
  RELOAD(game, config);
  RELOAD(game, save_names);
  RELOAD(game, shaders);
}

Game GAME;
Music MUSIC;

static Sound PLACE_SOUND;
static Texture2D BREAK_PROGRESS_TEXTURE;
static Texture2D TOOLTIP_TEXTURE;
static Texture2D CURSOR_TEXTURE;

void game_create_world(Game *game, float seed) {
  player_set_pos_ex(&game->player, TILE_SIZE * ((float)CHUNK_SIZE / 2), TILE_SIZE * ((float)CHUNK_SIZE / 2), false,
                    false, false);
  game->world.seed = seed;
  world_gen(&game->world);
}

// Uses null at the end to terminate
static const char *TEXTURE_MANAGER_TEXTURE_PATHS[TEXTURE_MANAGER_MAX_TEXTURES] = {"cursor", "gui/tool_tip", "breaking_overlay", "slot", NULL};

void game_feature_add(Game *game, GameFeature game_feature) {
  if (game->feature_store.game_features_amount < game->feature_store.game_features_capacity) {
    game->feature_store.game_features[game->feature_store.game_features_amount++] = game_feature;
  }
}

// TICKING

static void handle_tile_interaction(Game *game) {
  Vec2f mouse_pos = GetMousePosition();
  Vec2f mouse_world_pos = GetScreenToWorld2D(mouse_pos, game->player.cam);
  int x_index = floor_div(mouse_world_pos.x, TILE_SIZE);
  int y_index = floor_div(mouse_world_pos.y, TILE_SIZE);

  bool interaction_in_range =
      abs((int)game->player.box.x - x_index * TILE_SIZE) < CONFIG.interaction_range * TILE_SIZE &&
      abs((int)game->player.box.y - y_index * TILE_SIZE) < CONFIG.interaction_range * TILE_SIZE;

  bool slot_selected = game_slot_selected();

  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !slot_selected && interaction_in_range) {
    TileInstance *selected_tile = world_highest_tile_at(&game->world, vec2i(x_index, y_index));
    bool correct_tool = false;
    if (game->player.held_item.type.item_props.tool_props.break_categories.categories_amount > 0) {
      TileIdCategories tool_categories = game->player.held_item.type.item_props.tool_props.break_categories;
      TileIdCategories selected_tile_categories = tile_categories(&selected_tile->type);
      for (int i = 0; i < tool_categories.categories_amount; i++) {
        for (int j = 0; j < selected_tile_categories.categories_amount; j++) {
          if (tool_categories.categories_amount > 0 && selected_tile_categories.categories_amount > 0 &&
              tool_categories.categories[i] == selected_tile_categories.categories[j]) {
            correct_tool = true;
            break;
          }
        }
      }
    }

    if (selected_tile->type.id == TILE_EMPTY || selected_tile->type.tile_props.break_time < 0 || !correct_tool) {
      game->player.break_progress = -1;
      return;
    }

    if (game->player.last_broken_tile.type.layer == selected_tile->type.layer ||
        game->player.last_broken_tile.type.id == TILE_EMPTY) {

      TileInstance tile = *selected_tile;
      TraceLog(LOG_DEBUG, "Break x: %d, y: %d, break progress: %d, tile: %s", x_index * TILE_SIZE, y_index * TILE_SIZE,
               game->player.break_progress, tile_type_to_string(&tile.type));
      if (CheckCollisionPointRec(mouse_world_pos,
                                 rectf_from_dimf(x_index * TILE_SIZE, y_index * TILE_SIZE, selected_tile->box))) {
        if (game->player.break_tile_pos.x != x_index || game->player.break_tile_pos.y != y_index) {
          game->player.break_tile_pos = vec2i(x_index, y_index);
          game->player.break_progress = -1;
          return;
        }

        game->player.break_progress += game->player.held_item.type.item_props.tool_props.break_speed + 1;
        game->player.break_tile_pos = vec2i(x_index, y_index);
        game->player.break_tile = tile;
        if (game->player.break_progress >= tile.type.tile_props.break_time) {
          if (game->player.held_item.type.id == ITEM_HAMMER) {
            for (int y = -1; y <= 1; y++) {
              for (int x = -1; x <= 1; x++) {
                TilePos tile_pos = vec2i(x_index + x, y_index + y);
                TileInstance *tile_ptr = world_highest_tile_at(&game->world, tile_pos);
                TileInstance tile = TILE_INSTANCE_EMPTY;
                if (tile_ptr != NULL) {
                  tile = *tile_ptr;
                }
                world_remove_tile(&game->world, tile_pos);
                world_set_tile_on_layer(&game->world, tile_pos, tile_break_remainder(&tile, tile_pos), tile.type.layer);
              }
            }
          } else {
            TileInstance *tile_ptr = world_highest_tile_at(&game->world, vec2i(x_index, y_index));
            TileInstance tile = TILE_INSTANCE_EMPTY;
            if (tile_ptr != NULL) {
              tile = *tile_ptr;
            }
            TilePos tile_pos = vec2i(x_index, y_index);
            world_remove_tile(&game->world, tile_pos);
            world_set_tile_on_layer(&game->world, tile_pos, tile_break_remainder(&tile, tile_pos), tile.type.layer);
          }
          game->player.break_progress = -1;
          game->player.last_broken_tile = tile;
        }
      }
    }
  } else {
    game->player.break_progress = -1;
  }

  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    game->player.last_broken_tile = TILE_INSTANCE_EMPTY;
  }

  if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
    if (game->cur_menu == MENU_DEBUG) {
      DEBUG_GO_TO_POSITION = vec2f(x_index * TILE_SIZE, y_index * TILE_SIZE);
      TraceLog(LOG_DEBUG, "Set target position");
    }
  }

  if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && !slot_selected && interaction_in_range) {
    TileInstance *selected_tile = world_highest_tile_at(&game->world, vec2i(x_index, y_index));
    if (CheckCollisionPointRec(mouse_world_pos,
                               rectf_from_dimf(x_index * TILE_SIZE, y_index * TILE_SIZE, selected_tile->box))) {
      if (selected_tile->type.id == TILE_CHEST) {
        game_set_menu(game, MENU_DIALOG);
        return;
      }
      TileInstance new_tile = tile_new(game->debug_options.selected_tile_to_place_instance.type);
      bool placed = world_place_tile(&game->world, vec2i(x_index, y_index), new_tile);
      if (placed && game->sound_manager.sound_timer >= SOUND_COOLDOWN) {
        PlaySound(game->sound_manager.sound_buffer[game->sound_manager.cur_sound++]);
        if (game->sound_manager.cur_sound >= SOUND_BUFFER_LIMIT) {
          game->sound_manager.cur_sound = 0;
        }
        game->sound_manager.sound_timer = 0;
      }
    }
  }
}

static void handle_mouse_interaction(Game *game) {
  bool being_clicked = false;
  if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
    for (int i = 0; i < game->world.beings_amount; i++) {
      BeingInstance being = game->world.beings[i];
      if (being.id == BEING_NPC &&
          CheckCollisionPointRec(GetScreenToWorld2D(GetMousePosition(), game->player.cam), being.context.box)) {
        game_set_menu(game, MENU_DIALOG);
        TraceLog(LOG_DEBUG, "Clicked being");
        being_clicked = true;
        break;
      }
    }
  }

  if (!being_clicked) {
    handle_tile_interaction(game);
  }
}

static void handle_item_pickup(Game *game) {
  for (int i = 0; i < game->world.beings_amount; i++) {
    if (game->world.beings[i].id == BEING_ITEM &&
        CheckCollisionRecs(game->world.beings[i].context.box, player_collision_box(&game->player))) {
      if (GetTime() - game->world.beings[i].context.creation_time > CONFIG.item_pickup_delay) {
        world_remove_being(&game->world, &game->world.beings[i]);
        break;
      }
    }
  }
}

void game_world_tick(Game *game) {
  bool w = game->pressed_keys.move_backward_key;
  bool a = game->pressed_keys.move_left_key;
  bool s = game->pressed_keys.move_foreward_key;
  bool d = game->pressed_keys.move_right_key;

  player_tick(&game->player);

  player_handle_movement(&game->player, w, a, s, d);

  for (int i = 0; i < game->world.beings_amount; i++) {
    being_tick(&game->world.beings[i]);
  }

  handle_mouse_interaction(game);

  handle_item_pickup(game);

  game->sound_manager.sound_timer += GetFrameTime();

#ifdef SURTUR_DEBUG
  if (IsKeyPressed(KEY_F1)) {
    world_add_being(&game->world, being_npc_new(game->player.box.x, game->player.box.y));
    WORLD_BEING_ID = game->world.beings_amount - 1;
  }

  if (IsKeyPressed(KEYBINDS.open_close_debug_menu_key)) {
    if (game->cur_menu == MENU_NONE) {
      game_set_menu(game, MENU_DEBUG);
    } else {
      game_set_menu(game, MENU_NONE);
    }
  }
#endif
}

static void game_update_animations(void) {
  for (int i = 0; i < ANIMATED_TEXTURES_LEN; i++) {
    AnimatedTexture *texture = &ANIMATED_TEXTURES[i];
    texture->frame_timer += TICK_INTERVAL * 1000.0f;
    float delay = texture->texture.var.texture_animated.frame_time;
    if (texture->frame_timer >= delay) {
      int frames = texture->texture.var.texture_animated.frames;
      texture->cur_frame = (texture->cur_frame + 1) % frames;
      texture->frame_timer = 0;
    }
  }
}

void game_tick(Game *game) {
  game->slot_selected = game_slot_selected();

  UpdateMusicStream(MUSIC);

  game->window.width = GetScreenWidth();
  game->window.height = GetScreenHeight();

  if (game->window.width != game->window.prev_width || game->window.height != game->window.prev_height) {
    game->window.prev_width = game->window.width;
    game->window.prev_height = game->window.height;

    GAME.world_texture = LoadRenderTexture(game->window.width, game->window.height);

    camera_focus(&game->player.cam);
  }

  if (!game->paused) {
    game_world_tick(game);
  }

  if (IsKeyReleased(KEYBINDS.open_close_save_menu_key)) {
    if (game->cur_menu == MENU_SAVE) {
      game->cur_menu = MENU_NONE;
      game_set_menu(game, MENU_NONE);
      game->paused = false;
    } else if (game->cur_menu == MENU_NONE) {
      game_set_menu(game, MENU_SAVE);
      game->paused = true;
    }
  }

  if (IsKeyReleased(KEYBINDS.close_cur_menu) && game->cur_menu != MENU_NONE && game->cur_menu != MENU_SAVE) {
    game_set_menu(game, MENU_NONE);
  }

#ifdef SURTUR_DEBUG
  debug_tick();
#endif

  if (IsKeyPressed(KEYBINDS.reload_key)) {
    game_reload(&GAME);
  }

  game_update_animations();
  game->pressed_keys = (PressedKeys){};
}

static bool game_slot_selected() {
  Rectangle slot_rect = {.x = GetScreenWidth() - (3.5 * 16) - 30,
                         .y = (GetScreenHeight() / 2.0f) - (3.5 * 8),
                         .width = 20 * 3.5,
                         .height = 20 * 3.5};
  return CheckCollisionPointRec(GetMousePosition(), slot_rect);
}

// -- PARTICLES --