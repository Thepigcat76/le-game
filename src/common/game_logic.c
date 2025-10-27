#include "../../include/config.h"
#include "../../include/game.h"
#include "../../include/log.h"
#include <raylib.h>

static void game_world_tick(Game *game);

static void game_handle_mouse_interaction(Game *game);

static void game_handle_item_pickup(Game *game);

void game_tick(Game *game) {
  if (game->client_game != NULL) {
    client_tick(game->client_game);
  } else {
    //server_tick(game->server_game);
  }

  if (!game->client_game->paused && game->client_world != NULL) {
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

#ifdef DEBUG_BUILD
  debug_tick(&game->debug);
#endif

  if (IS_KEY_PRESSED(reload)) {
    client_reload(game->client_game);
    game_reload(game);
  }
}

static void game_world_tick(Game *game) {
  bool w = IS_KEY_DOWN(move_backward);
  bool a = IS_KEY_DOWN(move_left);
  bool s = IS_KEY_DOWN(move_foreward);
  bool d = IS_KEY_DOWN(move_right);

  player_tick(game->client_player);

  player_handle_movement(game->client_player, w, a, s, d);

  for (int i = 0; i < game->client_world->beings_amount; i++) {
    being_tick(&game->client_world->beings[i]);
  }

  game_handle_mouse_interaction(game);

  game_handle_item_pickup(game);

  // game->sound_manager.sound_timer += GetFrameTime();

  if (IS_KEY_PRESSED(open_close_inventory)) {
    if (game->client_game->cur_menu == MENU_NONE) {
      client_set_menu(game->client_game, MENU_INVENTORY);
    } else {
      client_set_menu(game->client_game, MENU_NONE);
    }
  }

#ifdef DEBUG_BUILD
  if (IsKeyPressed(KEY_F1)) {
    world_add_being(game->client_world, being_npc_new(game->client_player->box.x, game->client_player->box.y));
    game->debug.debug_controlled_being_id = game->client_world->beings_amount - 1;
  }

  if (IS_KEY_PRESSED(open_close_debug_menu)) {
    log_debug("F3 pressed");
    if (game->client_game->cur_menu == MENU_NONE) {
      client_set_menu(game->client_game, MENU_DEBUG);
    } else {
      client_set_menu(game->client_game, MENU_NONE);
    }
  }

  if (IS_KEY_PRESSED(visit_dungeon)) {
    game_enter_space(game, (SpaceDescriptor){.type = &SPACES[SPACE_DUNGEON_TEST], .id = 0});
  }
#endif
}

static TileIdCategories item_tile_categories(ItemInstance *item) { return item->type.item_props.tool_props.break_categories; }

static void game_handle_tile_interaction(Game *game) {
  Vec2f mouse_pos = GetMousePosition();
  Vec2f mouse_world_pos = GetScreenToWorld2D(mouse_pos, game->client_player->cam);
  int x_index = floor_div(mouse_world_pos.x, TILE_SIZE);
  int y_index = floor_div(mouse_world_pos.y, TILE_SIZE);

  bool interaction_in_range = abs((int)game->client_player->box.x - x_index * TILE_SIZE) < CONFIG.interaction_range * TILE_SIZE &&
      abs((int)game->client_player->box.y - y_index * TILE_SIZE) < CONFIG.interaction_range * TILE_SIZE;

  // Break tile
  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && true /*!slot_selected*/ && interaction_in_range) {
    TileInstance *selected_tile = world_highest_tile_at(game->client_world, vec2i(x_index, y_index));
    bool correct_tool = false;
    ItemInstance *player_held_item = &game->client_player->held_item;
    TileIdCategories tool_break_categories = item_tile_categories(player_held_item);
    // Check if tool has break categories
    if (tool_break_categories.categories_amount > 0) {
      TileIdCategories selected_tile_categories = tile_categories(game, selected_tile->type);
      if (selected_tile_categories.categories_amount > 0) {
        // Check if tool has correct tile category as the tile that should be broken
        for (int i = 0; i < tool_break_categories.categories_amount; i++) {
          for (int j = 0; j < selected_tile_categories.categories_amount; j++) {
            if (tool_break_categories.categories[i] == selected_tile_categories.categories[j]) {
              correct_tool = true;
              break;
            }
          }
        }
      }
    }

    if (selected_tile->type->id == TILE_EMPTY || selected_tile->type->tile_props.break_time < 0 || !correct_tool) {
      game->client_player->break_progress = -1;
      return;
    }

    if (game->client_player->last_broken_tile->type->layer == selected_tile->type->layer || game->client_player->last_broken_tile->type->id == TILE_EMPTY) {
      TraceLog(LOG_DEBUG, "Break x: %d, y: %d, break progress: %d, tile: %s", x_index * TILE_SIZE, y_index * TILE_SIZE,
               game->client_player->break_progress, tile_type_to_string(selected_tile->type));
      if (CheckCollisionPointRec(mouse_world_pos, rectf_from_dimf(x_index * TILE_SIZE, y_index * TILE_SIZE, selected_tile->box))) {
        if (game->client_player->break_tile_pos.x != x_index || game->client_player->break_tile_pos.y != y_index) {
          game->client_player->break_tile_pos = vec2i(x_index, y_index);
          game->client_player->break_progress = -1;
          return;
        }

        game->client_player->break_progress += game->client_player->held_item.type.item_props.tool_props.break_speed + 1;
        game->client_player->break_tile_pos = vec2i(x_index, y_index);
        log_debug("sleected tile: %p", selected_tile);
        game->client_player->break_tile = selected_tile;
        if (game->client_player->break_progress >= selected_tile->type->tile_props.break_time) {
          if (game->client_player->held_item.type.id == ITEM_HAMMER) {
            for (int y = -1; y <= 1; y++) {
              for (int x = -1; x <= 1; x++) {
                TilePos tile_pos = vec2i(x_index + x, y_index + y);
                TileInstance *tile_ptr = world_highest_tile_at(game->client_game->world, tile_pos);
                TileInstance tile = TILE_INSTANCE_EMPTY;
                if (tile_ptr != NULL) {
                  tile = *tile_ptr;
                }
                world_remove_tile(game->client_game->world, tile_pos);
                TileInstance remainder = tile_break_remainder(&tile, tile_pos);
                world_set_tile_on_layer(game->client_game->world, tile_pos, remainder, remainder.type->layer);
              }
            }
          } else {
            TileInstance *tile_ptr = world_highest_tile_at(game->client_game->world, vec2i(x_index, y_index));
            TileInstance tile = TILE_INSTANCE_EMPTY;
            if (tile_ptr != NULL) {
              tile = *tile_ptr;
            }
            TilePos tile_pos = vec2i(x_index, y_index);
            world_remove_tile(game->client_game->world, tile_pos);
            TileInstance remainder = tile_break_remainder(&tile, tile_pos);
            world_set_tile_on_layer(game->client_game->world, tile_pos, remainder, remainder.type->layer);
          }
          game->client_player->break_progress = -1;
          game->client_player->last_broken_tile = selected_tile;
        }
      }
    }
  } else {
    game->client_player->break_progress = -1;
  }

  // Reset last broken tile, which allows you to break any tile again
  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    game->client_player->last_broken_tile = &TILE_INSTANCE_EMPTY;
  }

  // Debug - set target position for npc to go to
  if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
    if (game->client_game->cur_menu == MENU_DEBUG) {
      game->debug.debug_go_to_pos = vec2f(x_index * TILE_SIZE, y_index * TILE_SIZE);
      TraceLog(LOG_DEBUG, "Set target position");
    }
  }

  // Place/Interact tile
  if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && true /*!slot_selected*/ && interaction_in_range) {
    TileInstance *selected_tile = world_highest_tile_at(game->client_world, vec2i(x_index, y_index));
    if (CheckCollisionPointRec(mouse_world_pos, rectf_from_dimf(x_index * TILE_SIZE, y_index * TILE_SIZE, selected_tile->box))) {
      if (selected_tile->type->id == TILE_CHEST) {
        // game_set_menu(game, MENU_DIALOG);
        return;
      }
      TileInstance new_tile = tile_new(game->debug.options.selected_tile_to_place_instance.type);
      if (!game->debug.options.print_tile_debug_info) {
        bool placed = world_place_tile(game->client_world, vec2i(x_index, y_index), new_tile);
      } else {
        char buf[512];
        tile_instance_debug(world_highest_tile_at(game->client_world, vec2i(x_index, y_index)), buf);
        puts(buf);
      }
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

static void game_handle_mouse_interaction(Game *game) {
  bool being_clicked = false;
  if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
    for (int i = 0; i < game->client_world->beings_amount; i++) {
      BeingInstance being = game->client_world->beings[i];
      if (being.id == BEING_NPC && CheckCollisionPointRec(GetScreenToWorld2D(GetMousePosition(), game->client_player->cam), being.context.box)) {
        // game_set_menu(game, MENU_DIALOG);
        TraceLog(LOG_DEBUG, "Clicked being");
        being_clicked = true;
        break;
      }
    }
  }

  if (!being_clicked) {
    game_handle_tile_interaction(game);
  }
}

static void game_handle_item_pickup(Game *game) {
  for (int i = 0; i < game->client_world->beings_amount; i++) {
    if (game->client_world->beings[i].id == BEING_ITEM &&
        CheckCollisionRecs(game->client_world->beings[i].context.box, player_collision_box(game->client_player))) {
      if (GetTime() - game->client_world->beings[i].context.creation_time > CONFIG.item_pickup_delay) {
        world_remove_being(game->client_world, &game->client_world->beings[i]);
        break;
      }
    }
  }
}