#include "../../include/tile.h"
#include "../../include/game.h"
#include "../../include/net/client.h"
#include "../../include/shared.h"
#include "../../include/textures.h"
#include "lilc/array.h"
#include <dirent.h>
#include <limits.h>
#include <raylib.h>
#include <stdio.h>
#include <string.h>

// TILE TYPE

static Bump ADV_TILE_BUMP;
AdvTileInstance *ADV_TILES;

#define INIT_TILE(src_file_name)                                                                                                           \
  extern void src_file_name##_tile_init();                                                                                                 \
  src_file_name##_tile_init();

#define TILE_REGISTER_CATEGORY(lookup_ptr, tile_id, ...)                                                                                   \
  {                                                                                                                                        \
    (lookup_ptr)->tiles[(lookup_ptr)->tiles_amount] = tile_id;                                                                             \
    (lookup_ptr)->tile_categories[(lookup_ptr)->tiles_amount] = (TileIdCategories)__VA_ARGS__;                                             \
    (lookup_ptr)->tiles_amount++;                                                                                                          \
  }

TileInstance TILE_INSTANCE_EMPTY = {0};

// TILE INSTANCE

void adv_tile_init(AdvTileInstance *adv_tile_inst) {
  // switch (type->id) {
  // case TILE_CHEST: {
  //   int index = array_len(ADV_TILES);
  //   AdvTileInstance adv_tile_instance = ((AdvTileInstance){.type = ADV_TILE_CHEST, .var = {}});
  //   array_add(ADV_TILES, adv_tile_instance);
  //   return &ADV_TILES[index];
  // }
  // default:
  //   return NULL;
  // }
}

TileLayer tile_layer_from_str(const char *layer_literal) {
  if (strcmp(layer_literal, "ground") == 0)
    return TILE_LAYER_GROUND;
  else if (strcmp(layer_literal, "top") == 0)
    return TILE_LAYER_TOP;
  PANIC_FMT("Failed to get layer from string: %s", layer_literal);
}

void tile_init(TileInstance *tile_inst, TileId id) {
  bool client = GAME_SIDE == SIDE_CLIENT;

  tile_inst->id = id;
  tile_inst->box = (Dimensionsf){.width = TILE_SIZE, .height = TILE_SIZE};
  // TODO: Write manager for advanced tiles
  adv_tile_init(tile_inst->adv_tile_instance);

  if (client && id != TILE_EMPTY) {
    TileProperties tile_props = CLIENT_GAME.game.registries.tiles[id];
    Vec2i default_pos = tile_default_sprite_pos();
    int default_sprite_res = tile_default_sprite_resolution();

    cw_Texture tex = tex_by_id(&CLIENT_GAME.asset_manager, tile_props.texture);

    tile_inst->cur_sprite_box = tile_props.uses_tileset ? rectf(default_pos.x, default_pos.y, default_sprite_res, default_sprite_res)
                                                        : rectf(0, 0, tex.width, tex.height);
    tile_inst->animation_frame = 0;
    // if (tex.has_meta_info && tex.meta_info.has_variants) {
    //   int max = tile_variants_amount_for_tile(type, 0, 0) - 1;
    //   if (max >= 0) {
    //     int r = GetRandomValue(0, max);
    //     tile.variant_texture = tile_variants_for_tile(type, 0, 0)[r];
    //   } else {
    //     // TODO: Properly fix this
    //     tile_inst->variant_texture = type->texture;
    //   }
    // }

    for (int j = 0; j < 8; j++) {
      tile_inst->texture_data.surrounding_tiles[j] = TILE_EMPTY;
    }

    tile_calc_sprite_box(tile_inst);
  }
}

void tile_instance_debug(const TileInstance *tile, char *buf) {
  // char tex_data_buf[256] = "";
  // strcat(tex_data_buf, "Surrounding tiles: [");
  // for (int i = 0; i < 8; i++) {
  //
  //   strcat(tex_data_buf, tile_type_to_string(&TILES[tile->texture_data.surrounding_tiles[i]]));
  //   strcat(tex_data_buf, ",");
  // }
  // strcat(tex_data_buf, "]");
  //  sprintf(buf,
  //          "-- %s --\n  box: {w: %d, h: %d}\n  adv_tile: %p\n  texture_data: %s\n  sprite box: {x: %f, y: %f, w: %f, h: %f}\n
  //          var_texture:
  //          "
  //          "%s\n  var_index: %d\n  anim_frame: %d",
  //          tile_type_to_string(tile->type), tile->box.width, tile->box.height, tile->adv_tile_instance, tex_data_buf,
  //          tile->cur_sprite_box.x, tile->cur_sprite_box.y, tile->cur_sprite_box.width, tile->cur_sprite_box.height,
  //          tile->variant_texture.path, tile->type->variant_index, tile->animation_frame);
}

Rectf tile_collision_box_at(const TileInstance *tile, int x, int y) {
  Vec2f offset = tile_collision_offset_at(tile);
  return rectf_from_dimf(x + offset.x, y + offset.y, tile_collision_dimensions_at(tile));
}

Dimensionsf tile_collision_dimensions_at(const TileInstance *tile) {
  TileProperties tile_props = CLIENT_GAME.game.registries.tiles[tile->id];

  if (tile_props.layer == TILE_LAYER_TOP) {
    switch (tile_props.id) {
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
  TileProperties tile_props = CLIENT_GAME.game.registries.tiles[tile->id];

  if (tile_props.layer == TILE_LAYER_TOP) {
    switch (tile_props.id) {
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
  TileProperties tile_props = CLIENT_GAME.game.registries.tiles[tile->id];

  if (tile_props.id == TILE_TREE) {
    TileInstance tile = {0};
    tile_init(&tile, TILE_TREE_STUMP);
    return tile;
  }
  return TILE_INSTANCE_EMPTY;
}

void tile_render_scaled(TileInstance *tile, int x, int y, float scale) {
  TileProperties tile_props = CLIENT_GAME.game.registries.tiles[tile->id];

  if (tile_props.has_texture) {
    // TODO: Reenable variant texture rendering
    if (false) {
      AssetId variant_tex_id = tile->variant_texture;
      cw_Texture variant_tex = tex_by_id(&CLIENT_GAME.asset_manager, variant_tex_id);
      DrawTextureRecEx(variant_tex.texture, tile->cur_sprite_box, vec2f(x, y), 0, scale, WHITE);
    } else {
      cw_Texture tex = tex_by_id(&CLIENT_GAME.asset_manager, tile_props.texture);
      i32 cur_frame = 0;             // cw_tex_cur_frame(&tex);
      i32 frame_height = tex.height; // cw_tex_frame_height(&tex);
      Rectangle sprite_rect = tile->cur_sprite_box;
      sprite_rect.y += frame_height * cur_frame;
      int offset_x = (tile_props.tile_dimensions.width - TILE_SIZE) / 2;
      int offset_y = tile_props.tile_dimensions.height - TILE_SIZE;
      DrawTextureRecEx(tex.texture, sprite_rect, vec2f(x - offset_x, y - offset_y), 0, scale, WHITE);
#ifdef DEBUG_BUILD
#include "../../include/game.h"
      if (CLIENT_GAME.game.debug.options.hitboxes_shown && tile->type->layer == TILE_LAYER_TOP) {
        rec_draw_outline(tile_collision_box_at(tile, x, y), GREEN);
      }
#endif
    }
  }
}

void tile_render(TileInstance *tile, int x, int y, bool dbg) {
  TileProperties tile_props = CLIENT_GAME.game.registries.tiles[tile->id];

  if (tile_props.has_texture) {
    cw_Texture tex = tile_tex(tile->id, vec2i(x, y), CLIENT_WORLD->seed, &CLIENT_GAME.tile_tex_manager, &CLIENT_GAME.game.registries,
                             &CLIENT_GAME.asset_manager);
    u32 cur_frame = cw_tex_cur_frame(&tex);
    u32 frame_height = cw_tex_frame_height(&tex);
    Rectangle sprite_rect = tile->cur_sprite_box;
    sprite_rect.y += frame_height * cur_frame;
    i32 offset_x = (tile_props.tile_dimensions.width - TILE_SIZE) / 2;
    i32 offset_y = tile_props.tile_dimensions.height - TILE_SIZE;
    DrawTextureRec(tex.texture, sprite_rect, vec2f(x - offset_x, y - offset_y), WHITE);
#ifdef DEBUG_BUILD
#include "../../include/game.h"
    if (CLIENT_GAME.game.debug.options.hitboxes_shown && tile->type->layer == TILE_LAYER_TOP) {
      rec_draw_outline(tile_collision_box_at(tile, x, y), GREEN);
    }
#endif
  }
}

void tile_right_click(TileInstance *tile) {}

void tile_tick(TileInstance *tile) {}

void tile_load(TileInstance *tile, const DataMap *data) {}

void tile_save(const TileInstance *tile, DataMap *data) {}
