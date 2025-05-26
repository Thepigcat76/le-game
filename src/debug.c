#include "../include/debug.h"
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

void debug_render() {
  switch (GAME.debug_options.game_object_display) {
  case DEBUG_DISPLAY_ALL_ITEMS: {
    for (int i = 0; i < ITEM_TYPE_AMOUNT; i++) {
      ItemInstance item = (ItemInstance){.type = ITEMS[i]};
      item_render(&item, ((float)SCREEN_WIDTH / 2) - (ITEM_TYPE_AMOUNT * 16 * 3.5) / 2 + (i * 20 * 3.5),
                  ((float)SCREEN_HEIGHT / 2) - 8 * 3.5);
    }
    break;
  }
  case DEBUG_DISPLAY_ALL_TILES: {
    for (int i = 0; i < TILE_TYPE_AMOUNT; i++) {
      TileInstance tile =
          tile_new(&TILES[i], ((float)SCREEN_WIDTH / 2) - (ITEM_TYPE_AMOUNT * 16 * 3.5) / 2 + (i * 20 * 3.5),
                   ((float)SCREEN_HEIGHT / 2) - 8 * 3.5);
      tile_render_scaled(&tile, 3.5);
    }
    break;
  }
  case DEBUG_DISPLAY_ALL_BEINGS: {
    float scale = 3;
    for (int i = 0; i < BEINGS_AMOUNT; i++) {
      int start_x = 0;
      BeingInstance being = DEBUG_BEINGS[i];
      TraceLog(LOG_DEBUG, "Id: %d", being.id);
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
    break;
  }
  }

  if (GAME.debug_options.hitboxes_shown) {
    Rectangle player_hitbox = GAME.player.box;
    player_hitbox.height = 6;
    player_hitbox.y += 26;
    rec_draw_outline(player_hitbox, BLUE);
    rec_draw_outline(rectf(GAME.player.tile_pos.x * TILE_SIZE, GAME.player.tile_pos.y * TILE_SIZE, 16, 16), RED);
  }
}

void debug_tick() {
  int keycode = GetKeyPressed();

  if (keycode >= KEY_ZERO && keycode <= KEY_NINE) {
    int tile_index = keycode - KEY_ZERO;
    if (tile_index < TILE_TYPE_AMOUNT) {
      GAME.debug_options.selected_tile_to_place_instance =
          tile_new(&TILES[tile_index], SELECTED_TILE_RENDER_POS.x + 35, SELECTED_TILE_RENDER_POS.y - 60);
    }
  }

  if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON) && GAME.cur_menu == MENU_DEBUG) {
    BeingInstance *being = &GAME.world.beings[WORLD_BEING_ID];
    being_brain_reset(being);
    being_add_activity(being,
                       (BeingActivity){.type = BEING_ACTIVITY_WALK_AROUND,
                                       .var = {.activity_walk_around = {.target_position = DEBUG_GO_TO_POSITION}}});
    TraceLog(LOG_DEBUG, "Added activity");
  }

}
