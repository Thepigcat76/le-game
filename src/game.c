#include "../include/game.h"
#include "../include/config.h"
#include <dirent.h>
#include <raylib.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>

#define COMMON_RELOAD(game_ptr, src_file_prefix)                                                                                           \
  extern void src_file_prefix##_on_reload(Game *game);                                                                                     \
  src_file_prefix##_on_reload(game_ptr)

void game_create(Game *game) {
  GAME = (Game){.debug_options = {.game_object_display = DEBUG_DISPLAY_NONE,
                                  .collisions_enabled = true,
                                  .hitboxes_shown = false,
                                  .selected_tile_to_place_instance = tile_new(TILES[TILE_DIRT])},
                .client_game = NULL,
                .server_game = NULL};
}

Game GAME;

void game_feature_add(Game *game, GameFeature game_feature) {
  if (game->cur_save.feature_store.game_features_amount < game->cur_save.feature_store.game_features_capacity) {
    game->cur_save.feature_store.game_features[game->cur_save.feature_store.game_features_amount++] = game_feature;
  }
}

void game_reload(Game *game) { COMMON_RELOAD(game, config); }

// TICKING

static void handle_tile_interaction(Game *game) {
  Vec2f mouse_pos = GetMousePosition();
  Vec2f mouse_world_pos = GetScreenToWorld2D(mouse_pos, game->player->cam);
  int x_index = floor_div(mouse_world_pos.x, TILE_SIZE);
  int y_index = floor_div(mouse_world_pos.y, TILE_SIZE);

  bool interaction_in_range = abs((int)game->player->box.x - x_index * TILE_SIZE) < CONFIG.interaction_range * TILE_SIZE &&
      abs((int)game->player->box.y - y_index * TILE_SIZE) < CONFIG.interaction_range * TILE_SIZE;

  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && true /*!slot_selected*/ && interaction_in_range) {
    TileInstance *selected_tile = world_highest_tile_at(game->world, vec2i(x_index, y_index));
    bool correct_tool = false;
    if (game->player->held_item.type.item_props.tool_props.break_categories.categories_amount > 0) {
      TileIdCategories tool_categories = game->player->held_item.type.item_props.tool_props.break_categories;
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
      game->player->break_progress = -1;
      return;
    }

    if (game->player->last_broken_tile.type.layer == selected_tile->type.layer || game->player->last_broken_tile.type.id == TILE_EMPTY) {

      TileInstance tile = *selected_tile;
      TraceLog(LOG_DEBUG, "Break x: %d, y: %d, break progress: %d, tile: %s", x_index * TILE_SIZE, y_index * TILE_SIZE,
               game->player->break_progress, tile_type_to_string(&tile.type));
      if (CheckCollisionPointRec(mouse_world_pos, rectf_from_dimf(x_index * TILE_SIZE, y_index * TILE_SIZE, selected_tile->box))) {
        if (game->player->break_tile_pos.x != x_index || game->player->break_tile_pos.y != y_index) {
          game->player->break_tile_pos = vec2i(x_index, y_index);
          game->player->break_progress = -1;
          return;
        }

        game->player->break_progress += game->player->held_item.type.item_props.tool_props.break_speed + 1;
        game->player->break_tile_pos = vec2i(x_index, y_index);
        game->player->break_tile = tile;
        if (game->player->break_progress >= tile.type.tile_props.break_time) {
          if (game->player->held_item.type.id == ITEM_HAMMER) {
            for (int y = -1; y <= 1; y++) {
              for (int x = -1; x <= 1; x++) {
                TilePos tile_pos = vec2i(x_index + x, y_index + y);
                TileInstance *tile_ptr = world_highest_tile_at(game->world, tile_pos);
                TileInstance tile = TILE_INSTANCE_EMPTY;
                if (tile_ptr != NULL) {
                  tile = *tile_ptr;
                }
                world_remove_tile(game->world, tile_pos);
                world_set_tile_on_layer(game->world, tile_pos, tile_break_remainder(&tile, tile_pos), tile.type.layer);
              }
            }
          } else {
            TileInstance *tile_ptr = world_highest_tile_at(game->world, vec2i(x_index, y_index));
            TileInstance tile = TILE_INSTANCE_EMPTY;
            if (tile_ptr != NULL) {
              tile = *tile_ptr;
            }
            TilePos tile_pos = vec2i(x_index, y_index);
            world_remove_tile(game->world, tile_pos);
            world_set_tile_on_layer(game->world, tile_pos, tile_break_remainder(&tile, tile_pos), tile.type.layer);
          }
          game->player->break_progress = -1;
          game->player->last_broken_tile = tile;
        }
      }
    }
  } else {
    game->player->break_progress = -1;
  }

  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    game->player->last_broken_tile = TILE_INSTANCE_EMPTY;
  }

  if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
    if (game->client_game->cur_menu == MENU_DEBUG) {
      DEBUG_GO_TO_POSITION = vec2f(x_index * TILE_SIZE, y_index * TILE_SIZE);
      TraceLog(LOG_DEBUG, "Set target position");
    }
  }

  if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && true /*!slot_selected*/ && interaction_in_range) {
    TileInstance *selected_tile = world_highest_tile_at(game->world, vec2i(x_index, y_index));
    if (CheckCollisionPointRec(mouse_world_pos, rectf_from_dimf(x_index * TILE_SIZE, y_index * TILE_SIZE, selected_tile->box))) {
      if (selected_tile->type.id == TILE_CHEST) {
        // game_set_menu(game, MENU_DIALOG);
        return;
      }
      TileInstance new_tile = tile_new(game->debug_options.selected_tile_to_place_instance.type);
      bool placed = world_place_tile(game->world, vec2i(x_index, y_index), new_tile);
      // if (placed && game->sound_manager.sound_timer >= SOUND_COOLDOWN) {
      //   PlaySound(game->sound_manager.sound_buffers[SOUND_PLACE].sound_buf[game->sound_manager.cur_sound++]);
      //   if (game->sound_manager.cur_sound >= SOUND_BUFFER_LIMIT) {
      //     game->sound_manager.cur_sound = 0;
      //   }
      //   game->sound_manager.sound_timer = 0;
      // }
    }
  }
}

static void handle_mouse_interaction(Game *game) {
  bool being_clicked = false;
  if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
    for (int i = 0; i < game->world->beings_amount; i++) {
      BeingInstance being = game->world->beings[i];
      if (being.id == BEING_NPC && CheckCollisionPointRec(GetScreenToWorld2D(GetMousePosition(), game->player->cam), being.context.box)) {
        // game_set_menu(game, MENU_DIALOG);
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
  for (int i = 0; i < game->world->beings_amount; i++) {
    if (game->world->beings[i].id == BEING_ITEM &&
        CheckCollisionRecs(game->world->beings[i].context.box, player_collision_box(game->player))) {
      if (GetTime() - game->world->beings[i].context.creation_time > CONFIG.item_pickup_delay) {
        world_remove_being(game->world, &game->world->beings[i]);
        break;
      }
    }
  }
}

static char *bts(bool b) { return b ? "true" : "false"; }

void game_world_tick(Game *game) {
  bool w = IS_KEY_DOWN(move_backward);
  bool a = IS_KEY_DOWN(move_left);
  bool s = IS_KEY_DOWN(move_foreward);
  bool d = IS_KEY_DOWN(move_right);

  player_tick(game->player);

  player_handle_movement(game->player, w, a, s, d);

  for (int i = 0; i < game->world->beings_amount; i++) {
    being_tick(&game->world->beings[i]);
  }

  handle_mouse_interaction(game);

  handle_item_pickup(game);

  // game->sound_manager.sound_timer += GetFrameTime();

#ifdef SURTUR_DEBUG
  if (IsKeyPressed(KEY_F1)) {
    world_add_being(game->world, being_npc_new(game->player->box.x, game->player->box.y));
    WORLD_BEING_ID = game->world->beings_amount - 1;
  }

  if (IS_KEY_PRESSED(open_close_debug_menu)) {
    printf("F3 pressed\n");
    if (game->client_game->cur_menu == MENU_NONE) {
      client_set_menu(game->client_game, MENU_DEBUG);
    } else {
      client_set_menu(game->client_game, MENU_NONE);
    }
  }
#endif
}

void game_tick(Game *game) {
  if (game->client_game != NULL) {
    client_tick(game->client_game);
  } else {
    // server_tick(game->server_game);
  }

  if (!game->client_game->paused && game->world != NULL) {
    game_world_tick(game);
  }

  if (IS_KEY_PRESSED(open_close_save_menu)) {
    if (game->client_game->cur_menu == MENU_SAVE) {
      client_set_menu(game->client_game, MENU_NONE);
      game->client_game->paused = false;
    } else if (game->client_game->cur_menu == MENU_NONE) {
      client_set_menu(game->client_game, MENU_SAVE);
      game->client_game->paused = true;
    }
  }

  if (IS_KEY_PRESSED(close_cur_menu) && game->client_game->cur_menu != MENU_NONE && game->client_game->cur_menu != MENU_SAVE) {
    // game_set_menu(game, MENU_NONE);
  }

#ifdef SURTUR_DEBUG
  debug_tick();
#endif

  if (IS_KEY_PRESSED(reload)) {
    client_reload(game->client_game);
    game_reload(game);
  }
}
