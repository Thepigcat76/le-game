#include "../include/debug.h"
#include "../include/game.h"

void debug_render() {
  switch (GAME.debug_options.game_object_display) {
  case DISPLAY_ALL_ITEMS: {
    for (int i = 0; i < ITEM_TYPE_AMOUNT; i++) {
      ItemInstance item = (ItemInstance){.type = ITEMS[i]};
      item_render(&item, ((float)SCREEN_WIDTH / 2) - (ITEM_TYPE_AMOUNT * 16 * 3.5) / 2 + (i * 20 * 3.5),
                  ((float)SCREEN_HEIGHT / 2) - 8 * 3.5);
    }
    break;
  }
  case DISPLAY_ALL_TILES: {
    for (int i = 0; i < TILE_TYPE_AMOUNT; i++) {
      TileInstance tile =
          tile_new(&TILES[i], ((float)SCREEN_WIDTH / 2) - (ITEM_TYPE_AMOUNT * 16 * 3.5) / 2 + (i * 20 * 3.5),
                   ((float)SCREEN_HEIGHT / 2) - 8 * 3.5);
      tile_render_scaled(&tile, 3.5);
    }
    break;
  }
  case DISPLAY_NONE: {
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
}
