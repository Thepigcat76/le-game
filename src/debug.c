#include "../include/debug.h"
#include "../include/config.h"
#include "../include/game.h"
#include "../include/keys.h"
#include "../include/net/client.h"
#include "../include/registries/beings.h"
#include "lilc/array.h"
#include "raylib.h"
#include "rlgl.h"

void debug_init(Debug *debug, struct _game *game) {
  debug->options = game->debug.options;
  debug->game = game;

  for (int i = 0; i < BEINGS_AMOUNT; i++) {
    debug->debug_beings[i] = being_new_default(i);
    debug->debug_beings_width += debug->debug_beings[i].context.box.width * 3;
  }
}

static void debug_render_game_object_overlay(Debug *debug) {
  switch (debug->game->debug.options.game_object_display) {
  case DEBUG_DISPLAY_ALL_ITEMS: {
    debug->game->client_game->state.paused = true;
    ItemProperties *item_props;
    array_foreach(debug->game->registries.items, item_props) {
      ItemInstance item = (ItemInstance){.id = item_props->id};
      float scale = 3.5;
      float x = ((float)SCREEN_WIDTH / 2) - (_amount_item_ids * 16 * scale) / 2 + (item_props->id * 20 * scale);
      float y = ((float)SCREEN_HEIGHT / 2) - 8 * scale;
      item_render(&item, x, y);
      cw_Texture tex = tex_by_id(&CLIENT_GAME.asset_manager, item_props->texture);
      Rectf item_box = rectf(x, y, tex.width * scale, tex.height * scale);
      rec_draw_outline(item_box, WHITE);
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), item_box)) {
        CLIENT_PLAYER->held_item = item;
      }
    }
    break;
  }
  case DEBUG_DISPLAY_ALL_TILES: {
    debug->game->client_game->state.paused = true;
    TileInstance *tile_inst;
    array_foreach(debug->options.selectable_tiles, tile_inst) {
      double x = ((float)SCREEN_WIDTH / 2) - (_amount_tile_ids * 16 * 3.5) / 2 + (tile_inst->id * 32 * 3.5);
      double y = ((float)SCREEN_HEIGHT / 2) - 8 * 3.5;
      tile_render_scaled(tile_inst, x - 160, y, 3.5);
      Rectf tile_box = rectf(x - 185, y - TILE_SIZE * 2, TILE_SIZE * 3.5, TILE_SIZE * 3.5);
      rec_draw_outline(tile_box, WHITE);
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), tile_box)) {
        debug->options.selected_tile_to_place_instance = *tile_inst;
      }
    }
    break;
  }
  case DEBUG_DISPLAY_ALL_BEINGS: {
    debug->game->client_game->state.paused = true;
    float scale = 3;
    for (int i = 0; i < BEINGS_AMOUNT; i++) {
      int start_x = 0;
      BeingInstance being = debug->debug_beings[i];
      being.context.box.x = ((float)(GetScreenWidth() - debug->debug_beings_width) / 2 + i * being.context.box.width * scale) / scale;
      being.context.box.y = 100;
      rlPushMatrix();
      {
        rlScalef(scale, scale, 1);
        being_render(&being);
      }
      rlPopMatrix();
    }
    break;
  }
  case DEBUG_DISPLAY_NONE: {
    debug->game->client_game->state.paused = false;
    break;
  }
  }
}

void debug_render_overlay(Debug *debug) {
  Vec2i selected_tile_render_pos = SELECTED_TILE_RENDER_POS(GetScreenWidth(), GetScreenHeight());
  tile_render_scaled(&debug->options.selected_tile_to_place_instance, selected_tile_render_pos.x + 35, selected_tile_render_pos.y - 60, 4);
  if (debug->game->client_game->state.cur_menu == MENU_DEBUG) {
    debug_render_game_object_overlay(debug);

    TileInstance *hovered_tile = debug->game->client_game->state.hovered_tile;
    TileProperties hovered_tile_props = debug->game->registries.tiles[hovered_tile->id];
    if (hovered_tile != NULL) {
      char *tile_name = hovered_tile_props.name;
      int font_size = CONFIG.default_font_size / 1.5;
      DrawText(tile_name, 0, 32, font_size, WHITE);
      DrawText(TextFormat("Sprite: %d, %d", (int)hovered_tile->cur_sprite_box.x, (int)hovered_tile->cur_sprite_box.y), 0, 32 + font_size,
               font_size, WHITE);
    }
  }
}

void debug_render(Debug *debug) {
  if (debug->game->debug.options.hitboxes_shown) {
    Rectangle player_hitbox = player_collision_box(CLIENT_PLAYER);
    rec_draw_outline(player_hitbox, BLUE);
    rec_draw_outline(rectf(CLIENT_PLAYER->tile_pos.x * TILE_SIZE, CLIENT_PLAYER->tile_pos.y * TILE_SIZE, 16, 16), RED);

    for (size_t i = 0; i < array_len(CLIENT_WORLD->beings); i++) {
      rec_draw_outline(CLIENT_WORLD->beings[i].context.box, WHITE);
    }
  }

  if (debug->game->client_game->state.cur_menu == MENU_DEBUG) {
    int id = debug->debug_controlled_being_id;
    BeingBrain brain = CLIENT_WORLD->beings[id].brain;
    // if (brain.activities_amount > 0) {
    //   BeingActivityWalkAround wa_activity = brain.activities[0].var.activity_walk_around;
    //   DrawCircleV(wa_activity.cur_target_pos, 8, WHITE);
    // }
  }
}

void debug_tick(Debug *debug) {
  int keycode = GetKeyPressed();

  if (keycode >= KEY_ZERO && keycode <= KEY_NINE) {
    int tile_index = keycode - KEY_ZERO;
    if (tile_index < _amount_tile_ids) {
      tile_init(&debug->options.selected_tile_to_place_instance, tile_index);
    }
  }

  if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON) && debug->game->client_game->state.cur_menu == MENU_DEBUG) {
    BeingInstance *being = &CLIENT_WORLD->beings[debug->debug_controlled_being_id];
    being_brain_reset(being);
    being_activities_add_walk_around(being, debug->debug_go_to_pos);
    TraceLog(LOG_DEBUG, "Added activity");
  }

  if (IsKeyReleased(KEYBINDS.close_cur_menu_key) && debug->game->client_game->state.cur_menu == MENU_DEBUG) {
    if (debug->options.game_object_display != DEBUG_DISPLAY_NONE) {
      debug->options.game_object_display = DEBUG_DISPLAY_NONE;
    } else {
      client_set_menu(&CLIENT_GAME, MENU_NONE);
    }
  }
}
