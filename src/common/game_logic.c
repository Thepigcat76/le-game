#include "../../include/config.h"
#include "../../include/game.h"
#include "../../include/net/client.h"
#include "../../include/reload.h"
#include "lilc/array.h"
#include "lilc/log.h"
#include <raylib.h>

static void game_world_tick(Game *game);

static void game_handle_mouse_interaction(Game *game);

static void game_handle_item_pickup(Game *game);

void game_tick(Game *game) {
  if (game->client_game != NULL) {
    client_tick(game->client_game);
  } else {
    // server_tick(game->server_game);
  }

  if (!game->client_game->state.paused && CLIENT_WORLD != NULL) {
    game_world_tick(game);
  }

  if (IS_KEY_PRESSED(open_close_save_menu)) {
    if (game->client_game->state.cur_menu == MENU_SAVE) {
      client_set_menu(game->client_game, MENU_NONE);
      game->client_game->state.paused = false;
    } else if (game->client_game->state.cur_menu == MENU_NONE) {
      client_set_menu(game->client_game, MENU_SAVE);
      game->client_game->state.paused = true;
    }
  }

  if (IS_KEY_PRESSED(close_cur_menu) && game->client_game->state.cur_menu != MENU_NONE && game->client_game->state.cur_menu != MENU_SAVE) {
    // game_set_menu(game, MENU_NONE);
  }

#ifdef DEBUG_BUILD
  debug_tick(&game->debug);
#endif

  if (IS_KEY_PRESSED(reload)) {
    client_reload(game->client_game);
    common_reload(game);

    log_info("CLIENT RELOADED");
  }
}

static void game_world_tick(Game *game) {
  bool w = IS_KEY_DOWN(move_backward);
  bool a = IS_KEY_DOWN(move_left);
  bool s = IS_KEY_DOWN(move_foreward);
  bool d = IS_KEY_DOWN(move_right);

  player_tick(CLIENT_PLAYER);

  player_handle_movement(CLIENT_PLAYER, w, a, s, d);

  BeingInstance *being;
  array_foreach(CLIENT_WORLD->beings, being) { being_tick(being); }

  game_handle_mouse_interaction(game);

  game_handle_item_pickup(game);

  // game->sound_manager.sound_timer += GetFrameTime();

  if (IS_KEY_PRESSED(open_close_inventory)) {
    if (game->client_game->state.cur_menu == MENU_NONE) {
      client_set_menu(game->client_game, MENU_INVENTORY);
    } else {
      client_set_menu(game->client_game, MENU_NONE);
    }
  }

#ifdef DEBUG_BUILD
  if (IsKeyPressed(KEY_F1)) {
    world_add_being(CLIENT_WORLD, being_npc_new(CLIENT_PLAYER->box.x, CLIENT_PLAYER->box.y));
    game->debug.debug_controlled_being_id = array_len(CLIENT_WORLD->beings) - 1;
  }

  if (IS_KEY_PRESSED(open_close_debug_menu)) {
    log_debug("F3 pressed");
    if (game->client_game->state.cur_menu == MENU_NONE) {
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

TileCategories item_tile_categories(const ItemInstance *item) {
  ItemProperties item_props = CLIENT_GAME.game.registries.items[item->id];
  return item_props.tool_props.break_categories;
}

bool item_tool_correct_for_tile(const ItemInstance *item, const TileInstance *tile, Category *lookup) {
  TileCategories tool_break_categories = item_tile_categories(item);
  if (tool_break_categories == NULL)
    return false;

  // Check if tool has break categories
  if (array_len(tool_break_categories) > 0) {
    char **category_name;
    array_foreach(tool_break_categories, category_name) {
      if (is_category_of_elem_id(lookup, tile->id, *category_name)) {
        return true;
      }
    }
  }

  return false;
}

static void game_handle_tile_interaction(Game *game) {
  Vec2f mouse_pos = GetMousePosition();
  Vec2f mouse_world_pos = GetScreenToWorld2D(mouse_pos, game->client_game->cam);
  int x_index = floor_div(mouse_world_pos.x, TILE_SIZE);
  int y_index = floor_div(mouse_world_pos.y, TILE_SIZE);

  bool interaction_in_range = abs((int)CLIENT_PLAYER->box.x - x_index * TILE_SIZE) < CONFIG.interaction_range * TILE_SIZE &&
      abs((int)CLIENT_PLAYER->box.y - y_index * TILE_SIZE) < CONFIG.interaction_range * TILE_SIZE;

  // Break tile
  if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && true /*!slot_selected*/ && interaction_in_range) {
    TileInstance *selected_tile = world_highest_tile_at(CLIENT_WORLD, vec2i(x_index, y_index));
    TileProperties selected_tile_props = game->registries.tiles[selected_tile->id];
    bool correct_tool = item_tool_correct_for_tile(&CLIENT_PLAYER->held_item, selected_tile, &game->tile_categories);

    if (selected_tile->id == TILE_EMPTY || selected_tile_props.break_time < 0 || !correct_tool) {
      CLIENT_PLAYER->break_progress = -1;
      return;
    }

    TileProperties last_broken_tile_props = game->registries.tiles[CLIENT_PLAYER->last_broken_tile->id];

    if (last_broken_tile_props.layer == selected_tile_props.layer || CLIENT_PLAYER->last_broken_tile->id == TILE_EMPTY) {
      TraceLog(LOG_DEBUG, "Break x: %d, y: %d, break progress: %d, tile: %s", x_index * TILE_SIZE, y_index * TILE_SIZE,
               CLIENT_PLAYER->break_progress, selected_tile_props.name);
      if (CheckCollisionPointRec(mouse_world_pos, rectf_from_dimf(x_index * TILE_SIZE, y_index * TILE_SIZE, selected_tile->box))) {
        if (CLIENT_PLAYER->break_tile_pos.x != x_index || CLIENT_PLAYER->break_tile_pos.y != y_index) {
          CLIENT_PLAYER->break_tile_pos = vec2i(x_index, y_index);
          CLIENT_PLAYER->break_progress = -1;
          return;
        }

        ItemProperties held_item_props = game->registries.items[CLIENT_PLAYER->held_item.id];
        CLIENT_PLAYER->break_progress += held_item_props.tool_props.break_speed + 1;
        CLIENT_PLAYER->break_tile_pos = vec2i(x_index, y_index);
        log_debug("sleected tile: %d", selected_tile->id);
        CLIENT_PLAYER->break_tile = selected_tile;
        if (CLIENT_PLAYER->break_progress >= selected_tile_props.break_time) {
          if (CLIENT_PLAYER->held_item.id == ITEM_HAMMER) {
            for (int y = -1; y <= 1; y++) {
              for (int x = -1; x <= 1; x++) {
                TilePos tile_pos = vec2i(x_index + x, y_index + y);
                TileInstance *tile_ptr = world_highest_tile_at(game->client_game->world, tile_pos);
                TileInstance tile = TILE_INST_EMPTY;
                if (tile_ptr != NULL) {
                  tile = *tile_ptr;
                }
                world_remove_tile(game->client_game->world, tile_pos);
                TileInstance remainder = tile_break_remainder(&tile, tile_pos);
                TileProperties remainder_tile_props = game->registries.tiles[remainder.id];
                world_set_tile_on_layer(game->client_game->world, tile_pos, remainder, remainder_tile_props.layer);
              }
            }
          } else {
            TileInstance *tile_ptr = world_highest_tile_at(game->client_game->world, vec2i(x_index, y_index));
            TileInstance tile = TILE_INST_EMPTY;
            if (tile_ptr != NULL) {
              tile = *tile_ptr;
            }
            TilePos tile_pos = vec2i(x_index, y_index);
            world_remove_tile(game->client_game->world, tile_pos);
            TileInstance remainder = tile_break_remainder(&tile, tile_pos);
            TileProperties remainder_tile_props = game->registries.tiles[remainder.id];
            world_set_tile_on_layer(game->client_game->world, tile_pos, remainder, remainder_tile_props.layer);
          }
          CLIENT_PLAYER->break_progress = -1;
          CLIENT_PLAYER->last_broken_tile = selected_tile;
        }
      }
    }
  } else {
    CLIENT_PLAYER->break_progress = -1;
  }

  // Reset last broken tile, which allows you to break any tile again
  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    CLIENT_PLAYER->last_broken_tile = &TILE_INST_EMPTY;
  }

  // Debug - set target position for npc to go to
  if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
    if (game->client_game->state.cur_menu == MENU_DEBUG) {
      game->debug.debug_go_to_pos = vec2f(x_index * TILE_SIZE, y_index * TILE_SIZE);
      TraceLog(LOG_DEBUG, "Set target position");
    }
  }

  // Place/Interact tile
  if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && true /*!slot_selected*/ && interaction_in_range) {
    TileInstance *selected_tile = world_highest_tile_at(CLIENT_WORLD, vec2i(x_index, y_index));
    if (CheckCollisionPointRec(mouse_world_pos, rectf_from_dimf(x_index * TILE_SIZE, y_index * TILE_SIZE, selected_tile->box))) {
      if (selected_tile->id == TILE_CHEST) {
        // game_set_menu(game, MENU_DIALOG);
        return;
      }
      TileInstance new_tile;
      tile_init(&new_tile, game->debug.options.selected_tile_to_place_instance.id);
      if (!game->debug.options.print_tile_debug_info) {
        bool placed = world_place_tile(CLIENT_WORLD, vec2i(x_index, y_index), new_tile);
      } else {
        char buf[512];
        tile_instance_debug(world_highest_tile_at(CLIENT_WORLD, vec2i(x_index, y_index)), buf);
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
    BeingInstance *being;
    array_foreach(CLIENT_WORLD->beings, being) {
      if (being->id == BEING_NPC &&
          CheckCollisionPointRec(GetScreenToWorld2D(GetMousePosition(), game->client_game->cam), being->context.box)) {
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
  BeingInstance *being;
  array_foreach(CLIENT_WORLD->beings, being) {
    if (being->id == BEING_ITEM && CheckCollisionRecs(being->context.box, player_collision_box(CLIENT_PLAYER))) {
      if (GetTime() - being->context.creation_time > CONFIG.item_pickup_delay) {
        ItemInstance item = being->extra.var.item_instance.item;
        ItemProperties item_props = game->registries.items[item.id];
        world_remove_being(CLIENT_WORLD, being);
        log_debug("Inserting item: %s", item_props.name);
        item_container_insert(&CLIENT_PLAYER->inv_container, item);
        break;
      }
    }
  }
}