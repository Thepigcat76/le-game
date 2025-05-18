#include "../include/tile.h"
#include "../include/shared.h"
#include <dirent.h>
#include <limits.h>
#include <raylib.h>

// TILE TYPE

#define INIT_TILE(src_file_name)                                               \
  extern void src_file_name##_tile_init();                                     \
  src_file_name##_tile_init();

void tile_types_init() {
  INIT_TILE(empty)
  INIT_TILE(dirt)
  INIT_TILE(grass)
  INIT_TILE(stone)

  TILE_INSTANCE_EMPTY = tile_new(&TILES[TILE_EMPTY], 0, 0);
}

char *tile_type_to_string(const TileType *type) {
  switch (type->id) {
  case TILE_EMPTY:
    return "empty";
  case TILE_GRASS:
    return "grass";
  case TILE_STONE:
    return "stone";
  case TILE_DIRT:
    return "dirt";
  }
}

// TILE INSTANCE

TileInstance tile_new(const TileType *type, int x, int y) {
  Vec2i default_pos =tile_default_sprite_pos();
  int default_sprite_res = tile_default_sprite_resolution();
  TileInstance instance = {
      .type = *type,
      .box = {.width = TILE_SIZE,
              .height = TILE_SIZE,
              .x = (float)x,
              .y = (float)y},
      .custom_data = {},
      .cur_sprite_box =
          type->uses_tileset
              ? rect(default_pos.x, default_pos.y, default_sprite_res,
                     default_sprite_res)
              : rect(0, 0, type->texture.width, type->texture.height),
  };

  if (type->id != TILE_EMPTY && type->variant_index != -1) {
    int max = tile_variants_amount_for_tile(type, 0, 0) - 1;
    int r = shared_random(0, max);
    instance.variant_texture = tile_variants_for_tile(type, 0, 0)[r];
  }
  return instance;
}

void tile_render(const TileInstance *tile) {
  if (tile->type.has_texture) {
    if (tile->type.variant_index != -1) {
      DrawTextureRec(tile->variant_texture, tile->cur_sprite_box,
                     (Vector2){tile->box.x, tile->box.y}, WHITE);
    } else {
      DrawTextureRec(tile->type.texture, tile->cur_sprite_box,
                     (Vector2){tile->box.x, tile->box.y}, WHITE);
    }
  }
}

void tile_right_click(TileInstance *tile) {}

void tile_tick(TileInstance *tile) {}

void tile_load(TileInstance *tile, const DataMap *data) {}

void tile_save(const TileInstance *tile, DataMap *data) {}
