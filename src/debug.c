#include "../include/debug.h"
#include "../include/keys.h"
#include "../include/net/client.h"
#include "../include/game.h"
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
  switch (GAME.debug.options.game_object_display) {
  case DEBUG_DISPLAY_ALL_ITEMS: {
    GAME.client_game->paused = true;
    for (int i = 0; i < ITEMS_AMOUNT; i++) {
      ItemInstance item = (ItemInstance){.type = ITEMS[i]};
      float scale = 3.5;
      float x = ((float)SCREEN_WIDTH / 2) - (ITEMS_AMOUNT * 16 * scale) / 2 + (i * 20 * scale);
      float y = ((float)SCREEN_HEIGHT / 2) - 8 * scale;
      item_render(&item, x, y);
      Rectf item_box = rectf(x, y, item.type.texture.width * scale, item.type.texture.height * scale);
      rec_draw_outline(item_box, WHITE);
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), item_box)) {
        GAME.player->held_item = item;
      }
    }
    break;
  }
  case DEBUG_DISPLAY_ALL_TILES: {
    GAME.client_game->paused = true;
    for (int i = 0; i < TILES_AMOUNT; i++) {
      double x = ((float)SCREEN_WIDTH / 2) - (ITEMS_AMOUNT * 16 * 3.5) / 2 + (i * 32 * 3.5);
      double y = ((float)SCREEN_HEIGHT / 2) - 8 * 3.5;
      tile_render_scaled(&GAME.debug.options.selectable_tiles[i], x - 160, y, 3.5);
      Rectf tile_box = rectf(x - 185, y - TILE_SIZE * 2, TILE_SIZE * 3.5, TILE_SIZE * 3.5);
      rec_draw_outline(tile_box, WHITE);
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), tile_box)) {
        GAME.debug.options.selected_tile_to_place_instance = GAME.debug.options.selectable_tiles[i];
      }
    }
    break;
  }
  case DEBUG_DISPLAY_ALL_BEINGS: {
    GAME.client_game->paused = true;
    float scale = 3;
    for (int i = 0; i < BEINGS_AMOUNT; i++) {
      int start_x = 0;
      BeingInstance being = debug->debug_beings[i];
      being.context.box.x =
          ((float)(GetScreenWidth() - debug->debug_beings_width) / 2 + i * being.context.box.width * scale) / scale;
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
    GAME.client_game->paused = false;
    break;
  }
  }
}

void debug_render_overlay(Debug *debug) {
  Vec2i selected_tile_render_pos = SELECTED_TILE_RENDER_POS(GetScreenWidth(), GetScreenHeight());
  tile_render_scaled(&GAME.debug.options.selected_tile_to_place_instance, selected_tile_render_pos.x + 35,
                     selected_tile_render_pos.y - 60, 4);
  if (GAME.client_game->cur_menu == MENU_DEBUG) {
    debug_render_game_object_overlay(debug);
  }
}

void debug_render(Debug *debug) {
  if (GAME.debug.options.hitboxes_shown) {
    Rectangle player_hitbox = player_collision_box(GAME.player);
    rec_draw_outline(player_hitbox, BLUE);
    rec_draw_outline(rectf(GAME.player->tile_pos.x * TILE_SIZE, GAME.player->tile_pos.y * TILE_SIZE, 16, 16), RED);

    for (int i = 0; i < GAME.world->beings_amount; i++) {
      rec_draw_outline(GAME.world->beings[i].context.box, WHITE);
    }
  }

  if (GAME.client_game->cur_menu == MENU_DEBUG) {
    int id = debug->debug_controlled_being_id;
    BeingBrain brain = GAME.world->beings[id].brain;
    //if (brain.activities_amount > 0) {
    //  BeingActivityWalkAround wa_activity = brain.activities[0].var.activity_walk_around;
    //  DrawCircleV(wa_activity.cur_target_pos, 8, WHITE);
    //}
  }
}

void debug_tick(Debug *debug) {
  int keycode = GetKeyPressed();

  if (keycode >= KEY_ZERO && keycode <= KEY_NINE) {
    int tile_index = keycode - KEY_ZERO;
    if (tile_index < TILES_AMOUNT) {
      GAME.debug.options.selected_tile_to_place_instance = tile_new(&TILES[tile_index]);
    }
  }

  if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON) && GAME.client_game->cur_menu == MENU_DEBUG) {
    BeingInstance *being = &GAME.world->beings[debug->debug_controlled_being_id];
    being_brain_reset(being);
    being_activities_add_walk_around(being, debug->debug_go_to_pos);
    TraceLog(LOG_DEBUG, "Added activity");
  }

  if (IsKeyReleased(KEYBINDS.close_cur_menu_key) && GAME.client_game->cur_menu == MENU_DEBUG) {
    if (GAME.debug.options.game_object_display != DEBUG_DISPLAY_NONE) {
      GAME.debug.options.game_object_display = DEBUG_DISPLAY_NONE;
    } else {
      client_set_menu(&CLIENT_GAME, MENU_NONE);
    }
  }
}
