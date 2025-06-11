#include "../include/tile.h"
#include "../include/game.h"
#include "../include/shared.h"
#include <dirent.h>
#include <limits.h>
#include <raylib.h>

// TILE TYPE

#define INIT_TILE(src_file_name)                                                                                       \
  extern void src_file_name##_tile_init();                                                                             \
  src_file_name##_tile_init();

#define TILE_REGISTER_CATEGORY(tile_id, ...)                                                                           \
  {                                                                                                                    \
    GAME.tile_category_lookup.tiles[GAME.tile_category_lookup.tiles_amount] = tile_id;                                 \
    GAME.tile_category_lookup.tile_categories[GAME.tile_category_lookup.tiles_amount] = (TileIdCategories)__VA_ARGS__; \
    GAME.tile_category_lookup.tiles_amount++;                                                                          \
  }

static void debug_category_lookup(TileCategoryLookup lookup) {
  for (int i = 0; i < lookup.tiles_amount; i++) {
    TraceLog(LOG_DEBUG, "Tile: %s - Categories:", tile_type_to_string(&TILES[lookup.tiles[i]]));
    for (int j = 0; j < lookup.tile_categories[i].categories_amount; j++) {
      TraceLog(LOG_DEBUG, "  Category: %s", tile_category_to_string(lookup.tile_categories[i].categories[j]));
    }
  }
}

TileType TILES[MAX_TILE_TYPES];
size_t TILES_AMOUNT = 0;
TileInstance TILE_INSTANCE_EMPTY;

void tile_types_init() {
  INIT_TILE(empty)
  INIT_TILE(dirt)
  INIT_TILE(grass)
  INIT_TILE(stone)
  INIT_TILE(water)
  INIT_TILE(simple_tiles)

  TILE_INSTANCE_EMPTY = tile_new(TILES[TILE_EMPTY]);
}

void tile_categories_init(void) {
  TILE_REGISTER_CATEGORY(TILE_WORKSTATION, {.categories = {TILE_CATEGORY_WOOD}, .categories_amount = 1});
  TILE_REGISTER_CATEGORY(TILE_TREE, {.categories = {TILE_CATEGORY_WOOD}, .categories_amount = 1});
  TILE_REGISTER_CATEGORY(TILE_OVEN, {.categories = {TILE_CATEGORY_STONE}, .categories_amount = 1});
  TILE_REGISTER_CATEGORY(TILE_STONE, {.categories = {TILE_CATEGORY_STONE}, .categories_amount = 1});

  debug_category_lookup(GAME.tile_category_lookup);
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
  case TILE_TREE_STUMP:
    return "tree_stump";
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
  Vec2f offset = tile_collision_offset_at(tile);
  return rectf_from_dimf(x + offset.x, y + offset.y, tile_collision_dimensions_at(tile));
}

Dimensionsf tile_collision_dimensions_at(const TileInstance *tile) {
  if (tile->type.layer == TILE_LAYER_TOP) {
    switch (tile->type.id) {
    case TILE_TREE:
    case TILE_TREE_STUMP:
      return dimf(tile->box.width, tile->box.height);
    default:
      return dimf(tile->box.width, tile->box.height - 8);
    }
  }
  return tile->box;
}

Vec2f tile_collision_offset_at(const TileInstance *tile) {
  if (tile->type.layer == TILE_LAYER_TOP) {
    switch (tile->type.id) {
    case TILE_TREE:
    case TILE_TREE_STUMP:
      return vec2f(0, -4);
    default:
      return vec2f(0, 8);
    }
  }
  return vec2f(0, 0);
}

TileInstance tile_break_remainder(const TileInstance *tile, TilePos pos) {
  if (tile->type.id == TILE_TREE) {
    return tile_new(TILES[TILE_TREE_STUMP]);
  }
  return TILE_INSTANCE_EMPTY;
}

void tile_render_scaled(TileInstance *tile, int x, int y, float scale) {
  if (tile->type.has_texture) {
    if (tile->type.variant_index != -1) {
      DrawTextureRecEx(adv_texture_to_texture(&tile->variant_texture), tile->cur_sprite_box, vec2f(x, y), 0, scale,
                       WHITE);
    } else {
      Texture2D texture = adv_texture_to_texture(&tile->type.texture);
      int cur_frame = adv_texture_cur_frame(&tile->type.texture);
      int frame_height = adv_texture_frame_height(&tile->type.texture);
      Rectangle sprite_rect = tile->cur_sprite_box;
      sprite_rect.y += frame_height * cur_frame;
      int offset_x = (tile->type.tile_width - TILE_SIZE) / 2;
      int offset_y = tile->type.tile_height - TILE_SIZE;
      if (tile->type.texture.type == TEXTURE_ANIMATED) {
        // TraceLog(LOG_DEBUG, "height: %d, cur_frame: %d", frame_height, cur_frame);
      }
      DrawTextureRecEx(texture, sprite_rect, vec2f(x - offset_x, y - offset_y), 0, scale, WHITE);
#ifdef SURTUR_DEBUG
#include "../include/game.h"
      if (GAME.debug_options.hitboxes_shown && tile->type.layer == TILE_LAYER_TOP) {
        rec_draw_outline(tile_collision_box_at(tile, x, y), GREEN);
      }
#endif
    }
  }
}

void tile_render(TileInstance *tile, int x, int y) {
  if (tile->type.has_texture) {
    if (tile->type.variant_index != -1) {
      DrawTextureRec(adv_texture_to_texture(&tile->variant_texture), tile->cur_sprite_box, vec2f(x, y), WHITE);
    } else {
      Texture2D texture = adv_texture_to_texture(&tile->type.texture);
      int cur_frame = adv_texture_cur_frame(&tile->type.texture);
      int frame_height = adv_texture_frame_height(&tile->type.texture);
      Rectangle sprite_rect = tile->cur_sprite_box;
      sprite_rect.y += frame_height * cur_frame;
      int offset_x = (tile->type.tile_width - TILE_SIZE) / 2;
      int offset_y = tile->type.tile_height - TILE_SIZE;
      if (tile->type.texture.type == TEXTURE_ANIMATED) {
        // TraceLog(LOG_DEBUG, "height: %d, cur_frame: %d", frame_height, cur_frame);
      }
      DrawTextureRec(texture, sprite_rect, vec2f(x - offset_x, y - offset_y), WHITE);
#ifdef SURTUR_DEBUG
#include "../include/game.h"
      if (GAME.debug_options.hitboxes_shown && tile->type.layer == TILE_LAYER_TOP) {
        rec_draw_outline(tile_collision_box_at(tile, x, y), GREEN);
      }
#endif
    }
  }
}

TileIdCategories tile_categories(const TileType *type) {
  for (int i = 0; i < GAME.tile_category_lookup.tiles_amount; i++) {
    TileId tile_id = GAME.tile_category_lookup.tiles[i];
    if (type->id == tile_id) {
      return GAME.tile_category_lookup.tile_categories[i];
    }
  }
  return (TileIdCategories){};
}

void tile_right_click(TileInstance *tile) {}

void tile_tick(TileInstance *tile) {}

void tile_load(TileInstance *tile, const DataMap *data) {}

void tile_save(const TileInstance *tile, DataMap *data) {}
