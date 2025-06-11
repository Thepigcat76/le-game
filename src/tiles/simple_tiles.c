#include "../../include/tile.h"
#include "../../include/tile_init_helper.h"
#include <raylib.h>
#include <stdbool.h>

void simple_tiles_tile_init() {
  TILE_TYPE_INIT(TILE_WORKSTATION, TILE_LAYER_TOP, NULL, "workstation", TILE_PROPS_DEFAULT_WITH_COLOR(BROWN));
  TILE_TYPE_INIT(TILE_OVEN, TILE_LAYER_TOP, NULL, "tall_oven", TILE_PROPS_DEFAULT_WITH_COLOR(GRAY));
  TILE_TYPE_INIT(TILE_TREE, TILE_LAYER_TOP, NULL, "tree_5", TILE_PROPS_DEFAULT_WITH_COLOR(BROWN));
  TILE_TYPE_INIT(TILE_TREE_STUMP, TILE_LAYER_TOP, NULL, "tree_stump", TILE_PROPS_DEFAULT_WITH_COLOR(BROWN));
/*TILE_TYPE_INIT({
    .id = TILE_OVEN,
    .texture_path = "res/assets/tall_oven.png",
    .texture = adv_texture_load("res/assets/tall_oven.png"),
    .has_texture = true,
    .is_solid = true,
    .layer = TILE_LAYER_TOP,
    .tile_color = GRAY,
    .tile_width = 16,
    .tile_height = 48,
    .tile_item = NULL,
    .break_time = 64,

    .is_ticking = false,
    .stores_custom_data = false,
    .uses_tileset = false,
    .has_animation = false,
});
TILE_TYPE_INIT({
    .id = TILE_TREE,
    .texture_path = "res/assets/tree_5.png",
    .texture = adv_texture_load("res/assets/tree_5.png"),
    .has_texture = true,
    .is_solid = true,
    .layer = TILE_LAYER_TOP,
    .tile_color = BROWN,
    .tile_width = 48,
    .tile_height = 80,
    .tile_item = NULL,
    .break_time = 256,

    .is_ticking = false,
    .stores_custom_data = false,
    .uses_tileset = false,
    .has_animation = false,
});
TILE_TYPE_INIT({
    .id = TILE_TREE_STUMP,
    .texture_path = "res/assets/tree_stump.png",
    .texture = adv_texture_load("res/assets/tree_stump.png"),
    .has_texture = true,
    .is_solid = true,
    .layer = TILE_LAYER_TOP,
    .tile_color = BROWN,
    .tile_width = 48,
    .tile_height = 32,
    .tile_item = NULL,
    .break_time = 256,

    .is_ticking = false,
    .stores_custom_data = false,
    .uses_tileset = false,
    .has_animation = false,
});
*/
}
