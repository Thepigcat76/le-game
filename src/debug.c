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
}
