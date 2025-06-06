#include "../include/tile.h"
#include "../include/shared.h"
#include <dirent.h>
#include <limits.h>
#include <raylib.h>

// TILE TYPE

#define INIT_TILE(src_file_name)                                                                                       \
  extern void src_file_name##_tile_init();                                                                             \
  src_file_name##_tile_init();

void tile_types_init() {
  INIT_TILE(empty)
  INIT_TILE(dirt)
  INIT_TILE(grass)
  INIT_TILE(stone)
  INIT_TILE(water)
  INIT_TILE(workstation)
  INIT_TILE(oven)
  INIT_TILE(tree)

  TILE_INSTANCE_EMPTY = tile_new(TILES[TILE_EMPTY]);
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
  case TILE_WATER:
    return "water";
  case TILE_WORKSTATION:
    return "workstation";
  case TILE_OVEN:
    return "oven";
  case TILE_TREE:
    return "tree";
  }
}

// TILE INSTANCE

TileInstance tile_new(TileType type) {
  Vec2i default_pos = tile_default_sprite_pos();
  int default_sprite_res = tile_default_sprite_resolution();
  TileInstance instance = {
      .type = type,
      .box = {.width = TILE_SIZE, .height = TILE_SIZE},
      .custom_data = {},
      .cur_sprite_box = type.uses_tileset ? rectf(default_pos.x, default_pos.y, default_sprite_res, default_sprite_res)
                                          : rectf(0, 0, type.texture.width, type.texture.height),
      .animation_frame = 0,
  };

  if (type.id != TILE_EMPTY && type.variant_index != -1) {
    int max = tile_variants_amount_for_tile(&type, 0, 0) - 1;
    if (max >= 0) {
      int r = GetRandomValue(0, max);
      instance.variant_texture = tile_variants_for_tile(&type, 0, 0)[r];
    } else {
      // TODO: Properly fix this
      instance.variant_texture = type.texture;
    }
  }
  return instance;
}

Rectf tile_collision_box_at(const TileInstance *tile, int x, int y) {
  if (tile->type.layer == TILE_LAYER_TOP) {
    return rectf(x, y + 8, tile->box.width, tile->box.height - 8);
  }
  return rectf_from_dimf(x, y, tile->box);
}

void tile_render_scaled(TileInstance *tile, int x, int y, float scale) {
  if (tile->type.has_texture) {
    if (tile->type.variant_index != -1) {
      DrawTextureRecEx(adv_texture_to_texture(tile->variant_texture), tile->cur_sprite_box, vec2f(x, y), 0, scale, WHITE);
    } else {
      Rectangle sprite_rect = tile->cur_sprite_box;
      if (tile->type.has_animation && tile->type.id == TILE_WATER) {
        int offset_y = 64 * TILE_ANIMATION_FRAMES[tile->type.id];
        sprite_rect.y += offset_y;
      }
      DrawTextureRecEx(adv_texture_to_texture(tile->type.texture), sprite_rect, vec2f(x, y), 0, scale, WHITE);
    }
  }
}

void tile_render(TileInstance *tile, int x, int y) {
  if (tile->type.has_texture) {
    if (tile->type.variant_index != -1) {
      DrawTextureRec(adv_texture_to_texture(tile->variant_texture), tile->cur_sprite_box, vec2f(x, y), WHITE);
    } else {
      Rectangle sprite_rect = tile->cur_sprite_box;
      if (tile->type.has_animation && tile->type.id == TILE_WATER) {
        int offset_y = 64 * TILE_ANIMATION_FRAMES[tile->type.id];
        sprite_rect.y += offset_y;
      }
      int offset_y = tile->type.tile_height - TILE_SIZE;
      DrawTextureRec(adv_texture_to_texture(tile->type.texture), sprite_rect, vec2f(x, y - offset_y), WHITE);
#ifdef SURTUR_DEBUG
#include "../include/game.h"
      if (GAME.debug_options.hitboxes_shown && tile->type.layer == TILE_LAYER_TOP) {
        rec_draw_outline(tile_collision_box_at(tile, x, y), GREEN);
      }
#endif
    }
  }
}

void tile_right_click(TileInstance *tile) {}

void tile_tick(TileInstance *tile) {}

void tile_load(TileInstance *tile, const DataMap *data) {}

void tile_save(const TileInstance *tile, DataMap *data) {}
