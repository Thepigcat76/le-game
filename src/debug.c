#include "../include/debug.h"
#include "../include/config.h"
#include "../include/game.h"
#include "raylib.h"
#include "rlgl.h"

static BeingInstance DEBUG_BEINGS[BEINGS_AMOUNT];
static int DEBUG_BEINGS_WIDTH = 0;
Vec2f DEBUG_GO_TO_POSITION = {.x = 0, .y = 0};
int WORLD_BEING_ID = 0;

void debug_init() {
  for (int i = 0; i < BEINGS_AMOUNT; i++) {
    DEBUG_BEINGS[i] = being_new_default(i);
    DEBUG_BEINGS_WIDTH += DEBUG_BEINGS[i].context.box.width * 3;
  }
}

static void debug_render_game_object_overlay() {
  switch (GAME.debug_options.game_object_display) {
  case DEBUG_DISPLAY_ALL_ITEMS: {
    GAME.paused = true;
    for (int i = 0; i < ITEM_TYPE_AMOUNT; i++) {
      ItemInstance item = (ItemInstance){.type = ITEMS[i]};
      float scale = 3.5;
      float x = ((float)SCREEN_WIDTH / 2) - (ITEM_TYPE_AMOUNT * 16 * scale) / 2 + (i * 20 * scale);
      float y = ((float)SCREEN_HEIGHT / 2) - 8 * scale;
      item_render(&item, x, y);
      Rectf item_box = rectf(x, y, item.type.texture.width * scale, item.type.texture.height * scale);
      rec_draw_outline(item_box, WHITE);
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), item_box)) {
        GAME.player.held_item = item;
      }
    }
    break;
  }
  case DEBUG_DISPLAY_ALL_TILES: {
    GAME.paused = true;
    for (int i = 0; i < TILE_TYPE_AMOUNT; i++) {
      double x = ((float)SCREEN_WIDTH / 2) - (ITEM_TYPE_AMOUNT * 16 * 3.5) / 2 + (i * 32 * 3.5);
      double y = ((float)SCREEN_HEIGHT / 2) - 8 * 3.5;
      TileInstance tile = tile_new(TILES[i]);
      tile_render_scaled(&tile, x - 160, y, 3.5);
      Rectf tile_box = rectf(x - 185, y - TILE_SIZE * 2, TILE_SIZE * 3.5, TILE_SIZE * 3.5);
      rec_draw_outline(tile_box, WHITE);
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), tile_box)) {
        GAME.debug_options.selected_tile_to_place_instance = tile_new(tile.type);
      }
    }
    break;
  }
  case DEBUG_DISPLAY_ALL_BEINGS: {
    GAME.paused = true;
    float scale = 3;
    for (int i = 0; i < BEINGS_AMOUNT; i++) {
      int start_x = 0;
      BeingInstance being = DEBUG_BEINGS[i];
      being.context.box.x =
          ((float)(GetScreenWidth() - DEBUG_BEINGS_WIDTH) / 2 + i * being.context.box.width * scale) / scale;
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
    GAME.paused = false;
    break;
  }
  }
}

void debug_render_overlay() {
  if (GAME.cur_menu == MENU_DEBUG) {
    debug_render_game_object_overlay();
  }
}

void debug_render() {
  if (GAME.debug_options.hitboxes_shown) {
    Rectangle player_hitbox = player_collision_box(&GAME.player);
    rec_draw_outline(player_hitbox, BLUE);
    rec_draw_outline(rectf(GAME.player.tile_pos.x * TILE_SIZE, GAME.player.tile_pos.y * TILE_SIZE, 16, 16), RED);

    for (int i = 0; i < GAME.world.beings_amount; i++) {
      rec_draw_outline(GAME.world.beings[i].context.box, WHITE);
    }
  }
}

void debug_tick() {
  int keycode = GetKeyPressed();

  if (keycode >= KEY_ZERO && keycode <= KEY_NINE) {
    int tile_index = keycode - KEY_ZERO;
    if (tile_index < TILE_TYPE_AMOUNT) {
      GAME.debug_options.selected_tile_to_place_instance = tile_new(TILES[tile_index]);
    }
  }

  if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON) && GAME.cur_menu == MENU_DEBUG) {
    BeingInstance *being = &GAME.world.beings[WORLD_BEING_ID];
    being_brain_reset(being);
    being_add_activity(being,
                       (BeingActivity){.type = BEING_ACTIVITY_WALK_AROUND,
                                       .var = {.activity_go_to_position = {.target_position = DEBUG_GO_TO_POSITION}}});
    TraceLog(LOG_DEBUG, "Added activity");
  }

  if (IsKeyReleased(KEYBINDS.close_cur_menu) && GAME.cur_menu == MENU_DEBUG) {
    if (GAME.debug_options.game_object_display != DEBUG_DISPLAY_NONE) {
      GAME.debug_options.game_object_display = DEBUG_DISPLAY_NONE;
    } else {
      game_set_menu(&GAME, MENU_NONE);
    }
  }
}
